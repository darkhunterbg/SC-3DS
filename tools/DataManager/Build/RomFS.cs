using DataManager.Build.N3DS;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

// Credit & Source: https://github.com/SciresM/RomFS-Builder

namespace DataManager.Build
{
	public class RomFSBuilder
	{
		private const int PADDING_ALIGN = 16;

		public static ulong Align(ulong input, ulong alignsize)
		{
			ulong output = input;
			if (output % alignsize != 0) {
				output += (alignsize - (output % alignsize));
			}
			return output;
		}
		public static string ByteArrayToString(byte[] input)
		{
			StringBuilder sb = new StringBuilder();
			foreach (byte b in input) {
				sb.Append(b.ToString("X2") + " ");
			}
			return sb.ToString();
		}

		public static void Build(string srcDir, string dstFile, AsyncOperation operation)
		{
			srcDir = Path.GetFullPath(Path.GetDirectoryName(srcDir));


			operation.ItemName = "Building RomFS";

			FileNameTable FNT = new FileNameTable(srcDir);
			RomfsFile[] RomFiles = new RomfsFile[FNT.NumFiles];
			LayoutManager.Input[] In = new LayoutManager.Input[FNT.NumFiles];
			operation.ItemName = "Creating Layout...";
			for (int i = 0; i < FNT.NumFiles; i++) {
				operation.SetProgress(i, FNT.NumFiles);
				In[i] = new LayoutManager.Input();
				In[i].FilePath = FNT.NameEntryTable[i].FullName;
				In[i].AlignmentSize = 0x10;
			}
			operation.SetProgress(1, 1);
			LayoutManager.Output[] Out = LayoutManager.Create(In);
			for (int i = 0; i < Out.Length; i++) {
				RomFiles[i] = new RomfsFile();
				RomFiles[i].Offset = Out[i].Offset;
				RomFiles[i].PathName = Out[i].FilePath.Replace(Path.GetFullPath(srcDir), "").Replace("\\", "/");
				RomFiles[i].FullName = Out[i].FilePath;
				RomFiles[i].Size = Out[i].Size;
			}


			using (MemoryStream memoryStream = new MemoryStream()) {
				operation.ItemName = "Creating RomFS MetaData...";
				MetaDataBuilder mdb = new MetaDataBuilder();
				mdb.BuildRomFSHeader(memoryStream, RomFiles, srcDir);
				MakeRomFSData(RomFiles, memoryStream, dstFile, operation);
			}
		}

		private static void MakeRomFSData(RomfsFile[] RomFiles, MemoryStream metadata, string outFile, AsyncOperation operation)
		{
			var tempFile = Path.GetRandomFileName();
			operation.ItemName = "Computing IVFC Header Data...";
			IVFCInfo ivfc = new IVFCInfo();
			ivfc.Levels = new IVFCLevel[3];
			for (int i = 0; i < ivfc.Levels.Length; i++) {
				ivfc.Levels[i] = new IVFCLevel();
				ivfc.Levels[i].BlockSize = 0x1000;
			}
			ivfc.Levels[2].DataLength = RomfsFile.GetDataBlockLength(RomFiles, (ulong)metadata.Length);
			ivfc.Levels[1].DataLength = (Align(ivfc.Levels[2].DataLength, ivfc.Levels[2].BlockSize) / ivfc.Levels[2].BlockSize) * 0x20; //0x20 per SHA256 hash
			ivfc.Levels[0].DataLength = (Align(ivfc.Levels[1].DataLength, ivfc.Levels[1].BlockSize) / ivfc.Levels[1].BlockSize) * 0x20; //0x20 per SHA256 hash
			ulong MasterHashLen = (Align(ivfc.Levels[0].DataLength, ivfc.Levels[0].BlockSize) / ivfc.Levels[0].BlockSize) * 0x20;
			ulong lofs = 0;
			for (int i = 0; i < ivfc.Levels.Length; i++) {
				ivfc.Levels[i].HashOffset = lofs;
				lofs += Align(ivfc.Levels[i].DataLength, ivfc.Levels[i].BlockSize);
			}
			uint IVFC_MAGIC = 0x43465649; //IVFC
			uint RESERVED = 0x0;
			uint HeaderLen = 0x5C;
			uint MEDIA_UNIT_SIZE = 0x200;
			byte[] SuperBlockHash = new byte[0x20];
			FileStream OutFileStream = new FileStream(tempFile, FileMode.Create, FileAccess.ReadWrite);
			try {
				OutFileStream.Seek(0, SeekOrigin.Begin);
				OutFileStream.Write(BitConverter.GetBytes(IVFC_MAGIC), 0, 0x4);
				OutFileStream.Write(BitConverter.GetBytes(0x10000), 0, 0x4);
				OutFileStream.Write(BitConverter.GetBytes(MasterHashLen), 0, 0x4);
				for (int i = 0; i < ivfc.Levels.Length; i++) {
					OutFileStream.Write(BitConverter.GetBytes(ivfc.Levels[i].HashOffset), 0, 0x8);
					OutFileStream.Write(BitConverter.GetBytes(ivfc.Levels[i].DataLength), 0, 0x8);
					OutFileStream.Write(BitConverter.GetBytes((int)(Math.Log(ivfc.Levels[i].BlockSize, 2))), 0, 0x4);
					OutFileStream.Write(BitConverter.GetBytes(RESERVED), 0, 0x4);
				}
				OutFileStream.Write(BitConverter.GetBytes(HeaderLen), 0, 0x4);
				//IVFC Header is Written.
				OutFileStream.Seek((long)Align(MasterHashLen + 0x60, ivfc.Levels[0].BlockSize), SeekOrigin.Begin);
				byte[] metadataArray = metadata.ToArray();
				OutFileStream.Write(metadataArray, 0, metadataArray.Length);
				long baseOfs = OutFileStream.Position;
				operation.ItemName = "Writing Level 2 Data...";
				operation.SetProgress(0, RomFiles.Length);

				for (int i = 0; i < RomFiles.Length; i++) {
					OutFileStream.Seek((long)(baseOfs + (long)RomFiles[i].Offset), SeekOrigin.Begin);
					using (FileStream inStream = new FileStream(RomFiles[i].FullName, FileMode.Open, FileAccess.Read)) {
						while (inStream.Position < inStream.Length) {
							byte[] buffer = new byte[inStream.Length - inStream.Position > 0x100000 ? 0x100000 : inStream.Length - inStream.Position];
							inStream.Read(buffer, 0, buffer.Length);
							OutFileStream.Write(buffer, 0, buffer.Length);
						}
					}
					operation.SetProgress(i + 1, RomFiles.Length);
				}
				long hashBaseOfs = (long)Align((ulong)OutFileStream.Position, ivfc.Levels[2].BlockSize);
				long hOfs = (long)Align(MasterHashLen, ivfc.Levels[0].BlockSize);
				long cOfs = hashBaseOfs + (long)ivfc.Levels[1].HashOffset;
				SHA256Managed sha = new SHA256Managed();


				for (int i = ivfc.Levels.Length - 1; i >= 0; i--) {


					operation.ItemName = "Computing Level " + i + " Hashes...";

					byte[] buffer = new byte[(int)ivfc.Levels[i].BlockSize];

					for (long ofs = 0; ofs < (long)ivfc.Levels[i].DataLength; ofs += ivfc.Levels[i].BlockSize) {
						operation.SetProgress((int)(ofs / ivfc.Levels[i].BlockSize), (int)(ivfc.Levels[i].DataLength / ivfc.Levels[i].BlockSize));

						OutFileStream.Seek(hOfs, SeekOrigin.Begin);
						OutFileStream.Read(buffer, 0, (int)ivfc.Levels[i].BlockSize);
						hOfs = OutFileStream.Position;
						byte[] hash = sha.ComputeHash(buffer);
						OutFileStream.Seek(cOfs, SeekOrigin.Begin);
						OutFileStream.Write(hash, 0, hash.Length);
						cOfs = OutFileStream.Position;

					}
					operation.SetProgress(1, 1);

					if (i == 2) {
						long len = OutFileStream.Position;
						if (len % 0x1000 != 0) {
							len = (long)Align((ulong)len, 0x1000);
							byte[] buf = new byte[len - OutFileStream.Position];
							OutFileStream.Write(buf, 0, buf.Length);
						}
					}
					if (i > 0) {
						hOfs = hashBaseOfs + (long)ivfc.Levels[i - 1].HashOffset;
						if (i > 1) {
							cOfs = hashBaseOfs + (long)ivfc.Levels[i - 2].HashOffset;
						} else {
							cOfs = (long)Align(HeaderLen, PADDING_ALIGN);
						}
					}
				}
				OutFileStream.Seek(0, SeekOrigin.Begin);
				uint SuperBlockLen = (uint)Align(MasterHashLen + 0x60, MEDIA_UNIT_SIZE);
				byte[] MasterHashes = new byte[SuperBlockLen];
				OutFileStream.Read(MasterHashes, 0, (int)SuperBlockLen);
				SuperBlockHash = sha.ComputeHash(MasterHashes);
			} finally {
				if (OutFileStream != null)
					OutFileStream.Dispose();
			}
			//operation.ItemName = "RomFS Super Block Hash: " + ByteArrayToString(SuperBlockHash);
			WriteBinary(tempFile, outFile, operation);
		}
		private static void WriteBinary(string tempFile, string outFile, AsyncOperation operation)
		{
			operation.ItemName = "Writing Binary RomFS";

			using (FileStream fs = new FileStream(outFile, FileMode.Create)) {
				using (BinaryWriter writer = new BinaryWriter(fs)) {
					using (FileStream fileStream = new FileStream(tempFile, FileMode.Open, FileAccess.Read)) {
						uint BUFFER_SIZE = 0x400000; // 4MB Buffer

						operation.SetProgress(0, (int)(fileStream.Length / BUFFER_SIZE));

						int i = 0;
						byte[] buffer = new byte[BUFFER_SIZE];
						while (true) {
							int count = fileStream.Read(buffer, 0, buffer.Length);
							if (count != 0) {
								writer.Write(buffer, 0, count);
								operation.SetProgress(i++, (int)(fileStream.Length / BUFFER_SIZE));
							} else
								break;
						}
					}
					writer.Flush();
				}
			}
			File.Delete(tempFile);
		}
	}

}

namespace DataManager.Build.N3DS
{
	public class FileNameTable
	{
		public List<FileInfo> NameEntryTable { get; private set; }

		public int NumFiles
		{
			get
			{
				return this.NameEntryTable.Count;
			}
		}

		internal FileNameTable(string rootPath)
		{
			this.NameEntryTable = new List<FileInfo>();
			this.AddDirectory(new DirectoryInfo(rootPath));
		}

		internal void AddDirectory(DirectoryInfo dir)
		{
			foreach (FileInfo fileInfo in dir.GetFiles()) {
				this.NameEntryTable.Add(fileInfo);
			}
			foreach (DirectoryInfo subdir in dir.GetDirectories()) {
				this.AddDirectory(subdir);
			}
		}
	}
	public class RomfsFile
	{
		public string PathName;
		public ulong Offset;
		public ulong Size;
		public string FullName;

		public static ulong GetDataBlockLength(RomfsFile[] files, ulong PreData)
		{
			return (files.Length == 0) ? PreData : PreData + files[files.Length - 1].Offset + files[files.Length - 1].Size;
		}
	}
	public class IVFCInfo
	{
		public IVFCLevel[] Levels;
	}
	public class IVFCLevel
	{
		public ulong HashOffset;
		public ulong DataLength;
		public uint BlockSize;
	}

	public class LayoutManager
	{
		public static LayoutManager.Output[] Create(LayoutManager.Input[] Input)
		{
			List<LayoutManager.Output> list = new List<LayoutManager.Output>();
			ulong Len = 0;
			foreach (LayoutManager.Input input in Input) {
				LayoutManager.Output output = new LayoutManager.Output();
				FileInfo fileInfo = new FileInfo(input.FilePath);
				ulong ofs = RomFSBuilder.Align(Len, input.AlignmentSize);
				output.FilePath = input.FilePath;
				output.Offset = ofs;
				output.Size = (ulong)fileInfo.Length;
				list.Add(output);
				Len = ofs + (ulong)fileInfo.Length;
			}
			return list.ToArray();
		}

		public class Input
		{
			public string FilePath;
			public uint AlignmentSize;
		}

		public class Output
		{
			public string FilePath;
			public ulong Offset;
			public ulong Size;
		}
	}

	// ============================ MetaDataBuilder ==============================

	class MetaDataBuilder
	{
		public string ROOT_DIR;
		private const int PADDING_ALIGN = 16;
		private const uint ROMFS_UNUSED_ENTRY = 0xFFFFFFFF;

		public void BuildRomFSHeader(MemoryStream romfs_stream, RomfsFile[] Entries, string DIR)
		{
			ROOT_DIR = DIR;

			Romfs_MetaData MetaData = new Romfs_MetaData();

			InitializeMetaData(MetaData);

			CalcRomfsSize(MetaData);

			PopulateRomfs(MetaData, Entries);

			WriteMetaDataToStream(MetaData, romfs_stream);
		}

		private void InitializeMetaData(Romfs_MetaData MetaData)
		{
			MetaData.InfoHeader = new Romfs_InfoHeader();
			MetaData.DirTable = new Romfs_DirTable();
			MetaData.DirTableLen = 0;
			MetaData.M_DirTableLen = 0;
			MetaData.FileTable = new Romfs_FileTable();
			MetaData.FileTableLen = 0;
			MetaData.DirTable.DirectoryTable = new List<Romfs_DirEntry>();
			MetaData.FileTable.FileTable = new List<Romfs_FileEntry>();
			MetaData.InfoHeader.HeaderLength = 0x28;
			MetaData.InfoHeader.Sections = new Romfs_SectionHeader[4];
			MetaData.DirHashTable = new List<uint>();
			MetaData.FileHashTable = new List<uint>();
		}

		private void CalcRomfsSize(Romfs_MetaData MetaData)
		{
			MetaData.DirNum = 1;
			DirectoryInfo Root_DI = new DirectoryInfo(ROOT_DIR);
			CalcDirSize(MetaData, Root_DI);


			MetaData.M_DirHashTableEntry = GetHashTableEntryCount(MetaData.DirNum);

			MetaData.M_FileHashTableEntry = GetHashTableEntryCount(MetaData.FileNum);


			uint MetaDataSize = (uint)Align((ulong)(0x28 + MetaData.M_DirHashTableEntry * 4 + MetaData.M_DirTableLen + MetaData.M_FileHashTableEntry * 4 + MetaData.M_FileTableLen), PADDING_ALIGN);
			for (int i = 0; i < MetaData.M_DirHashTableEntry; i++) {
				MetaData.DirHashTable.Add(ROMFS_UNUSED_ENTRY);
			}
			for (int i = 0; i < MetaData.M_FileHashTableEntry; i++) {
				MetaData.FileHashTable.Add(ROMFS_UNUSED_ENTRY);
			}
			uint Pos = MetaData.InfoHeader.HeaderLength;
			for (int i = 0; i < 4; i++) {
				MetaData.InfoHeader.Sections[i].Offset = Pos;
				uint size = 0;
				switch (i) {
					case 0:
						size = MetaData.M_DirHashTableEntry * 4;
						break;
					case 1:
						size = MetaData.M_DirTableLen;
						break;
					case 2:
						size = MetaData.M_FileHashTableEntry * 4;
						break;
					case 3:
						size = MetaData.M_FileTableLen;
						break;
				}
				MetaData.InfoHeader.Sections[i].Size = size;
				Pos += size;
			}
			MetaData.InfoHeader.DataOffset = MetaDataSize;
		}

		private uint GetHashTableEntryCount(uint Entries)
		{
			uint count = Entries;
			if (Entries < 3)
				count = 3;
			else if (count < 19)
				count |= 1;
			else {
				while (count % 2 == 0 || count % 3 == 0 || count % 5 == 0 || count % 7 == 0 || count % 11 == 0 || count % 13 == 0 || count % 17 == 0) {
					count++;
				}
			}
			return count;
		}

		private void CalcDirSize(Romfs_MetaData MetaData, DirectoryInfo dir)
		{
			if (MetaData.M_DirTableLen == 0) {
				MetaData.M_DirTableLen = 0x18;
			} else {
				MetaData.M_DirTableLen += 0x18 + (uint)Align((ulong)dir.Name.Length * 2, 4);
			}

			FileInfo[] files = dir.GetFiles();
			for (int i = 0; i < files.Length; i++) {
				MetaData.M_FileTableLen += 0x20 + (uint)Align((ulong)files[i].Name.Length * 2, 4);
			}

			DirectoryInfo[] SubDirectories = dir.GetDirectories();
			for (int i = 0; i < SubDirectories.Length; i++) {
				CalcDirSize(MetaData, SubDirectories[i]);
			}

			MetaData.FileNum += (uint)files.Length;
			MetaData.DirNum += (uint)SubDirectories.Length;
		}

		private void PopulateRomfs(Romfs_MetaData MetaData, RomfsFile[] Entries)
		{
			//Recursively Add All Directories to DirectoryTable
			AddDir(MetaData, new DirectoryInfo(ROOT_DIR), 0, ROMFS_UNUSED_ENTRY);

			//Iteratively Add All Files to FileTable
			AddFiles(MetaData, Entries);

			//Set HashKeyPointers, Build HashTables
			PopulateHashTables(MetaData);

			//Thats it.
		}

		private void PopulateHashTables(Romfs_MetaData MetaData)
		{
			for (int i = 0; i < MetaData.DirTable.DirectoryTable.Count; i++) {
				AddDirHashKey(MetaData, i);
			}
			for (int i = 0; i < MetaData.FileTable.FileTable.Count; i++) {
				AddFileHashKey(MetaData, i);
			}
		}

		private void AddDirHashKey(Romfs_MetaData MetaData, int index)
		{
			uint parent = MetaData.DirTable.DirectoryTable[index].ParentOffset;
			string Name = MetaData.DirTable.DirectoryTable[index].Name;
			byte[] NArr = (index == 0) ? Encoding.Unicode.GetBytes("") : Encoding.Unicode.GetBytes(Name);
			uint hash = CalcPathHash(parent, NArr, 0, NArr.Length);
			int ind2 = (int)(hash % MetaData.M_DirHashTableEntry);
			if (MetaData.DirHashTable[ind2] == ROMFS_UNUSED_ENTRY) {
				MetaData.DirHashTable[ind2] = MetaData.DirTable.DirectoryTable[index].Offset;
			} else {
				int i = GetRomfsDirEntry(MetaData, MetaData.DirHashTable[ind2]);
				int tempindex = index;
				MetaData.DirHashTable[ind2] = MetaData.DirTable.DirectoryTable[index].Offset;
				while (true) {
					if (MetaData.DirTable.DirectoryTable[tempindex].HashKeyPointer == ROMFS_UNUSED_ENTRY) {
						MetaData.DirTable.DirectoryTable[tempindex].HashKeyPointer = MetaData.DirTable.DirectoryTable[i].Offset;
						break;
					} else {
						i = tempindex;
						tempindex = GetRomfsDirEntry(MetaData, MetaData.DirTable.DirectoryTable[i].HashKeyPointer);
					}
				}
			}
		}

		private void AddFileHashKey(Romfs_MetaData MetaData, int index)
		{
			uint parent = MetaData.FileTable.FileTable[index].ParentDirOffset;
			string Name = MetaData.FileTable.FileTable[index].Name;
			byte[] NArr = Encoding.Unicode.GetBytes(Name);
			uint hash = CalcPathHash(parent, NArr, 0, NArr.Length);
			int ind2 = (int)(hash % MetaData.M_FileHashTableEntry);
			if (MetaData.FileHashTable[ind2] == ROMFS_UNUSED_ENTRY) {
				MetaData.FileHashTable[ind2] = MetaData.FileTable.FileTable[index].Offset;
			} else {
				int i = GetRomfsFileEntry(MetaData, MetaData.FileHashTable[ind2]);
				int tempindex = index;
				MetaData.FileHashTable[ind2] = MetaData.FileTable.FileTable[index].Offset;
				while (true) {
					if (MetaData.FileTable.FileTable[tempindex].HashKeyPointer == ROMFS_UNUSED_ENTRY) {
						MetaData.FileTable.FileTable[tempindex].HashKeyPointer = MetaData.FileTable.FileTable[i].Offset;
						break;
					} else {
						i = tempindex;
						tempindex = GetRomfsFileEntry(MetaData, MetaData.FileTable.FileTable[i].HashKeyPointer);
					}
				}
			}
		}

		private uint CalcPathHash(uint ParentOffset, byte[] NameArray, int start, int len)
		{
			uint hash = ParentOffset ^ 123456789;
			for (int i = 0; i < NameArray.Length; i += 2) {
				hash = (uint)((hash >> 5) | (hash << 27));
				hash ^= (ushort)((NameArray[start + i]) | (NameArray[start + i + 1] << 8));
			}
			return hash;
		}

		private void AddDir(Romfs_MetaData MetaData, DirectoryInfo Dir, uint parent, uint sibling)
		{
			AddDir(MetaData, Dir, parent, sibling, false);
			AddDir(MetaData, Dir, parent, sibling, true);
		}

		private void AddDir(Romfs_MetaData MetaData, DirectoryInfo Dir, uint parent, uint sibling, bool DoSubs)
		{
			DirectoryInfo[] SubDirectories = Dir.GetDirectories();
			if (!DoSubs) {
				uint CurrentDir = MetaData.DirTableLen;
				Romfs_DirEntry Entry = new Romfs_DirEntry();
				Entry.ParentOffset = parent;
				Entry.ChildOffset = Entry.HashKeyPointer = Entry.FileOffset = ROMFS_UNUSED_ENTRY;
				Entry.SiblingOffset = sibling;
				Entry.FullName = Dir.FullName;
				Entry.Name = (Entry.FullName == ROOT_DIR) ? "" : Dir.Name;
				Entry.Offset = CurrentDir;
				MetaData.DirTable.DirectoryTable.Add(Entry);
				MetaData.DirTableLen += (CurrentDir == 0) ? 0x18 : 0x18 + (uint)Align((ulong)Dir.Name.Length * 2, 4);
				int ParentIndex = GetRomfsDirEntry(MetaData, Dir.FullName);
				uint poff = MetaData.DirTable.DirectoryTable[ParentIndex].Offset;
			} else {
				int CurIndex = GetRomfsDirEntry(MetaData, Dir.FullName);
				uint CurrentDir = MetaData.DirTable.DirectoryTable[CurIndex].Offset;
				for (int i = 0; i < SubDirectories.Length; i++) {
					AddDir(MetaData, SubDirectories[i], CurrentDir, sibling, false);
					if (i > 0) {
						string PrevFullName = SubDirectories[i - 1].FullName;
						string ThisName = SubDirectories[i].FullName;
						int PrevIndex = GetRomfsDirEntry(MetaData, PrevFullName);
						int ThisIndex = GetRomfsDirEntry(MetaData, ThisName);
						MetaData.DirTable.DirectoryTable[PrevIndex].SiblingOffset = MetaData.DirTable.DirectoryTable[ThisIndex].Offset;
					}
				}
				for (int i = 0; i < SubDirectories.Length; i++) {
					AddDir(MetaData, SubDirectories[i], CurrentDir, sibling, true);
				}
			}
			if (SubDirectories.Length > 0) {
				int curindex = GetRomfsDirEntry(MetaData, Dir.FullName);
				int childindex = GetRomfsDirEntry(MetaData, SubDirectories[0].FullName);
				if (curindex > -1 && childindex > -1)
					MetaData.DirTable.DirectoryTable[curindex].ChildOffset = MetaData.DirTable.DirectoryTable[childindex].Offset;
			}
		}

		private void AddFiles(Romfs_MetaData MetaData, RomfsFile[] Entries)
		{
			string PrevDirPath = "";
			for (int i = 0; i < Entries.Length; i++) {
				FileInfo file = new FileInfo(Entries[i].FullName);
				Romfs_FileEntry Entry = new Romfs_FileEntry();
				string DirPath = Path.GetDirectoryName(Entries[i].FullName);
				int ParentIndex = GetRomfsDirEntry(MetaData, DirPath);
				Entry.FullName = Entries[i].FullName;
				Entry.Offset = MetaData.FileTableLen;
				Entry.ParentDirOffset = MetaData.DirTable.DirectoryTable[ParentIndex].Offset;
				Entry.SiblingOffset = ROMFS_UNUSED_ENTRY;
				if (DirPath == PrevDirPath) {
					MetaData.FileTable.FileTable[i - 1].SiblingOffset = Entry.Offset;
				}
				if (MetaData.DirTable.DirectoryTable[ParentIndex].FileOffset == ROMFS_UNUSED_ENTRY) {
					MetaData.DirTable.DirectoryTable[ParentIndex].FileOffset = Entry.Offset;
				}
				Entry.HashKeyPointer = ROMFS_UNUSED_ENTRY;
				Entry.NameSize = (uint)file.Name.Length * 2;
				Entry.Name = file.Name;
				Entry.DataOffset = Entries[i].Offset;
				Entry.DataSize = Entries[i].Size;
				MetaData.FileTable.FileTable.Add(Entry);
				MetaData.FileTableLen += 0x20 + (uint)Align((ulong)file.Name.Length * 2, 4);
				PrevDirPath = DirPath;
			}
		}

		private void WriteMetaDataToStream(Romfs_MetaData MetaData, MemoryStream stream)
		{
			//First, InfoHeader.
			stream.Write(BitConverter.GetBytes(MetaData.InfoHeader.HeaderLength), 0, 4);
			foreach (Romfs_SectionHeader SH in MetaData.InfoHeader.Sections) {
				stream.Write(BitConverter.GetBytes(SH.Offset), 0, 4);
				stream.Write(BitConverter.GetBytes(SH.Size), 0, 4);
			}
			stream.Write(BitConverter.GetBytes(MetaData.InfoHeader.DataOffset), 0, 4);

			//DirHashTable
			foreach (uint u in MetaData.DirHashTable) {
				stream.Write(BitConverter.GetBytes(u), 0, 4);
			}

			//DirTable
			foreach (Romfs_DirEntry dir in MetaData.DirTable.DirectoryTable) {
				stream.Write(BitConverter.GetBytes(dir.ParentOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(dir.SiblingOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(dir.ChildOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(dir.FileOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(dir.HashKeyPointer), 0, 4);
				uint nlen = (uint)dir.Name.Length * 2;
				stream.Write(BitConverter.GetBytes(nlen), 0, 4);
				byte[] NameArray = new byte[(int)Align((ulong)nlen, 4)];
				Array.Copy(Encoding.Unicode.GetBytes(dir.Name), 0, NameArray, 0, nlen);
				stream.Write(NameArray, 0, NameArray.Length);
			}

			//FileHashTable
			foreach (uint u in MetaData.FileHashTable) {
				stream.Write(BitConverter.GetBytes(u), 0, 4);
			}

			//FileTable
			foreach (Romfs_FileEntry file in MetaData.FileTable.FileTable) {
				stream.Write(BitConverter.GetBytes(file.ParentDirOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(file.SiblingOffset), 0, 4);
				stream.Write(BitConverter.GetBytes(file.DataOffset), 0, 8);
				stream.Write(BitConverter.GetBytes(file.DataSize), 0, 8);
				stream.Write(BitConverter.GetBytes(file.HashKeyPointer), 0, 4);
				uint nlen = (uint)file.Name.Length * 2;
				stream.Write(BitConverter.GetBytes(nlen), 0, 4);
				byte[] NameArray = new byte[(int)Align((ulong)nlen, 4)];
				Array.Copy(Encoding.Unicode.GetBytes(file.Name), 0, NameArray, 0, nlen);
				stream.Write(NameArray, 0, NameArray.Length);
			}

			//Padding
			while (stream.Position % PADDING_ALIGN != 0)
				stream.Write(new byte[PADDING_ALIGN - (stream.Position % 0x10)], 0, (int)(PADDING_ALIGN - (stream.Position % 0x10)));
			//All Done.
		}

		//GetRomfs[...]Entry Functions are all O(n)

		private int GetRomfsDirEntry(Romfs_MetaData MetaData, string FullName)
		{
			for (int i = 0; i < MetaData.DirTable.DirectoryTable.Count; i++) {
				if (MetaData.DirTable.DirectoryTable[i].FullName == FullName) {
					return i;
				}
			}
			return -1;
		}

		private int GetRomfsDirEntry(Romfs_MetaData MetaData, uint Offset)
		{
			for (int i = 0; i < MetaData.DirTable.DirectoryTable.Count; i++) {
				if (MetaData.DirTable.DirectoryTable[i].Offset == Offset) {
					return i;
				}
			}
			return -1;
		}

		private int GetRomfsFileEntry(Romfs_MetaData MetaData, uint Offset)
		{
			for (int i = 0; i < MetaData.FileTable.FileTable.Count; i++) {
				if (MetaData.FileTable.FileTable[i].Offset == Offset) {
					return i;
				}
			}
			return -1;
		}

		private ulong Align(ulong input, ulong alignsize)
		{
			ulong output = input;
			if (output % alignsize != 0) {
				output += (alignsize - (output % alignsize));
			}
			return output;
		}

		public class Romfs_MetaData
		{
			public Romfs_InfoHeader InfoHeader;
			public uint DirNum;
			public uint FileNum;
			public List<uint> DirHashTable;
			public uint M_DirHashTableEntry;
			public Romfs_DirTable DirTable;
			public uint DirTableLen;
			public uint M_DirTableLen;
			public List<uint> FileHashTable;
			public uint M_FileHashTableEntry;
			public Romfs_FileTable FileTable;
			public uint FileTableLen;
			public uint M_FileTableLen;
		}

		public struct Romfs_SectionHeader
		{
			public uint Offset;
			public uint Size;
		}

		public struct Romfs_InfoHeader
		{
			public uint HeaderLength;
			public Romfs_SectionHeader[] Sections;
			public uint DataOffset;
		}

		public class Romfs_DirTable
		{
			public List<Romfs_DirEntry> DirectoryTable;
		}

		public class Romfs_FileTable
		{
			public List<Romfs_FileEntry> FileTable;
		}

		public class Romfs_DirEntry
		{
			public uint ParentOffset;
			public uint SiblingOffset;
			public uint ChildOffset;
			public uint FileOffset;
			public uint HashKeyPointer;
			public string Name;
			public string FullName;
			public uint Offset;
		}

		public class Romfs_FileEntry
		{
			public uint ParentDirOffset;
			public uint SiblingOffset;
			public ulong DataOffset;
			public ulong DataSize;
			public uint HashKeyPointer;
			public uint NameSize;
			public string Name;
			public string FullName;
			public uint Offset;
		}
	}
}
