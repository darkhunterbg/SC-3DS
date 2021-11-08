using DataManager.Assets;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Db = DataManager.Gameplay.AnimClipInstructionDatabase;

namespace DataManager.Gameplay
{


	public class IScriptParser
	{
		public class Animation
		{
			public string StartLabel;
			public List<string> Labels = new List<string>();
			public string Text = string.Empty;

			public List<string> GetInstructions()
			{
				return Text.Split(Environment.NewLine, StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries).ToList();
			}
		}

		public static readonly Dictionary<string, AnimationType> AnimationsMap = new Dictionary<string, AnimationType>()
		{
			{ "Init", AnimationType.Init },
			{"Death", AnimationType.Death },
			{ "GndAttkInit", AnimationType.GroundAttackInit },
			{ "AirAttkInit", AnimationType.AirAttackInit },
			{ "GndAttkRpt", AnimationType.GroundAttackRepeat },
			{ "AirAttkRpt", AnimationType.AirAttackRepeat },
			{ "CastSpell", AnimationType.CastSpell },
			{ "GndAttkToIdle", AnimationType.AirAttackInit },
			{ "AirAttkToIdle", AnimationType.AirAttackInit },
			{"Walking", AnimationType.Walking },
			{"WalkingToIdle", AnimationType.WalkingToIdle },
			{"SpecialState1", AnimationType.SpecialAbility1 }
		};

		public readonly List<string> Errors = new List<string>();

		public readonly Dictionary<AnimationType, Animation> Animations = new Dictionary<AnimationType, Animation>();

		class Label
		{
			public readonly StringBuilder Sb = new StringBuilder();
			public string Name;
			public int Position;
			public readonly List<string> References = new List<string>();
		}

		private readonly Dictionary<string, Label> Labels = new Dictionary<string, Label>();

		private Label _currentLabel;

		public void Parse(string text)
		{
			Errors.Clear();
			Animations.Clear();
			Labels.Clear();

			var lines = text.Split(new string[] { "\r", "\n" }, StringSplitOptions.None).ToList();

			int start = lines.IndexOf(".headerstart");
			int end = lines.IndexOf(".headerend");

			for (int i = start + 1; i < end; ++i) {
				string l = lines[i];

				if (string.IsNullOrWhiteSpace(l)) continue;

				if (l.StartsWith("#")) continue;

				if (l.StartsWith("IsId") || l.StartsWith("Type") || l.StartsWith("Unused")) continue;

				var s = l.Split(new string[] { "\t", " " }, StringSplitOptions.RemoveEmptyEntries);
				string animName = s[0];

				if (!AnimationsMap.TryGetValue(animName, out AnimationType type)) {
					Errors.Add($"[{i}] Unknown animation type '{animName}'");
					continue;
				}

				if (s[1] != "[NONE]")
					Animations[type] = new Animation() { StartLabel = s[1] };
			}

			List<Label> buildingLabels = new List<Label>();

			for (int i = end + 1; i < lines.Count; ++i) {
				string l = lines[i];
				if (l.StartsWith("#")) continue;

				if (string.IsNullOrWhiteSpace(l))
					continue;

				if (l.EndsWith(":")) {

					string labelName = l.Split(":")[0];

					if (_currentLabel != null) {
						_currentLabel.References.Add(labelName);
					}

					_currentLabel = new Label() { Name = labelName, Position = i };

					Labels[_currentLabel.Name] = _currentLabel;

					buildingLabels.Add(_currentLabel);

					foreach (var label in buildingLabels) {
						label.Sb.AppendLine($"{_currentLabel.Name}:");
					}



					continue;
				}

				var s = l.Split(new string[] { "\t", " ", "," }, StringSplitOptions.RemoveEmptyEntries);

				string name = s[0];
				List<string> parameters = s.Skip(1).ToList();

				string instruction = ConvertInstructtion(i, name, parameters);

				if (instruction != "DONE")
					foreach (var label in buildingLabels) {
						label.Sb.AppendLine(instruction);
					}

				if (instruction.StartsWith("goto ") || instruction == "destroy" || instruction == "DONE") {

					foreach (var anim in Animations) {
						int index = buildingLabels.IndexOf(label => label.Name == anim.Value.StartLabel);
						if (index == -1) continue;
						anim.Value.Labels.AddRange(buildingLabels.Skip(index).Select(label => label.Name));
					}

					buildingLabels.Clear();
					_currentLabel = null;
				}
			}


			foreach (var anim in Animations)
				GenerateAnimationText(anim.Value);
		}

		private void GenerateAnimationText(Animation anim)
		{

			anim.Text = Labels[anim.StartLabel].Sb.ToString();


			List<Label> refs = new List<Label>();

			Queue<string> search = new Queue<string>();
			search.Enqueue(anim.StartLabel);

			while (search.Count > 0) {
				var l = search.Dequeue();

				foreach (var r in Labels[l].References) {
					if (refs.Any(a => a.Name == r))
						continue;
					refs.Add(Labels[r]);
					search.Enqueue(r);
				}
			}

			refs.RemoveAll(l => anim.Labels.Contains(l.Name));

			//List<int> offset = new List<int>();

			foreach (var l in refs) {
				anim.Text += Environment.NewLine + l.Sb.ToString();
			}

			//int start = Labels[anim.StartLabel].Position;
			//foreach (var l in anim.Labels) {
			//	var label = Labels[l];
			//	int pos = label.Position - start;
			//	pos -= Labels.Values.Count(s => s.Position > start && s.Position <= label.Position);

			//	anim.Text = anim.Text.Replace($"[{label.Name}]", $"{pos}");
			//}
		}


		private void AddLabelRef(string label)
		{
			_currentLabel.References.Add(label);
		}

		private static SoundSetAsset GetSound(string name)
		{
			string soundName = name.ToLower().Replace(".wav", string.Empty);
			var def = AppGame.AssetManager.GetAssets<SoundSetAsset>()
				.FirstOrDefault(s => s.Clips.Any(c => c.Id.ToLower().EndsWith(soundName)));
			return def;
		}


		private string ConvertInstructtion(int line, string name, List<string> parameters)
		{
			switch (name) {
				case "imgul": _currentLabel.Position++; return string.Empty;
				case "playfram": return $"{Db.Frame} {Convert.ToInt32(parameters[0], 16)}";
				case "waitrand": return $"{Db.WaitRandom} {int.Parse(parameters[0])} {int.Parse(parameters[1])}";
				case "goto": AddLabelRef(parameters[0]); return $"{Db.GoTo} {parameters[0]}";
				case "randcondjmp": AddLabelRef(parameters[1]); return $"{Db.GoToRandom} {(int.Parse(parameters[0]) * 100) / 255} {parameters[1]}";
				case "wait": return $"{Db.Wait} {int.Parse(parameters[0])}";
				case "turnccwise": return $"{Db.TurnCounterkClockWise} {int.Parse(parameters[0])}";
				case "turncwise": return $"{Db.TurnClockWise} {int.Parse(parameters[0])}";
				case "playsndrand": {
						string soundName = parameters[4].ToLower();
						var def = GetSound(soundName);

						if (def == null) {
							Errors.Add($"[{line}] Sound set for '{soundName}' was not found.");
							break;
						}

						return $"{Db.PlaySound } {def.Path} 0";
					};
				case "playsndbtwn": {
						string soundName = parameters[3].ToLower();
						var def = GetSound(soundName);

						if (def == null) {
							Errors.Add($"[{line}] Sound set for '{soundName}' was not found.");
							break;
						}

						return $"{Db.PlaySound } {def.Path} 0";
					};
				case "playsnd": {
						string soundName = parameters[2].ToLower();
						var def = GetSound(soundName);

						if (def == null) {
							Errors.Add($"[{line}] Sound set for '{soundName}' was not found.");
							break;
						}

						return $"{Db.PlaySound } {def.Path} 0";
					}
				case "setfldirect": return $"{Db.Face} {int.Parse(parameters[0])}";
				case "lowsprul": {
						string spriteName = parameters[5].Replace("(", string.Empty).Replace(")", string.Empty)
							.Replace(".grp", string.Empty).ToLower();
						spriteName = $"unit\\{spriteName}";
						var def = AppGame.AssetManager.GetAssets<SpriteAsset>()
							.FirstOrDefault(s => s.Image.Key.ToLower() == spriteName);

						if (def == null) {
							Errors.Add($"[{line}] Sprite  for '{spriteName}' was not found.");
							break;
						}

						return $"{Db.SpawnSpriteBackground } {def.Path}";
					}
				case "end": return $"{Db.Destroy}";

				case "nobrkcodestart": return $"{Db.SetAIEnabled} false";
				case "nobrkcodeend": return $"{Db.SetAIEnabled} true";
				case "attackwith": return $"{Db.Attack}";
				case "attackmelee": {
						string soundName = parameters[3].ToLower();
						var def = GetSound(soundName);

						if (def == null) {
							Errors.Add($"[{line}] Sound set for '{soundName}' was not found.");
							break;
						}

						return $"{Db.PlaySound } {def.Path} 0{Environment.NewLine}{Db.Attack}";
					}
				case "gotorepeatattk": {
						return string.Empty;
						//return $"{Db.GoTo} {_currentLabel.Name}";
					}
				case "ignorerest": return "DONE";
				case "move": return $"{Db.Move} {int.Parse(parameters[0])}";
				default: {
						Errors.Add($"[{line}] Unknown instruction '{name}'");
						break;
					}
			}

			return name + " " + string.Join(" ", parameters);
		}
	}
}
