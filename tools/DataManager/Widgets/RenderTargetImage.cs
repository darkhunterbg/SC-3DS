using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace DataManager.Widgets
{
	public class RenderTargetImage : IDisposable
	{
		public IntPtr GuiImage { get; private set; }

		public RenderTarget2D RenderTarget { get; private set; }

		public Vector2 Size => new Vector2(RenderTarget.Width, RenderTarget.Height);

		public RenderTargetImage( Vector2 size)
		{
			RenderTarget = new RenderTarget2D(AppGame.Device, (int)size.X, (int)size.Y);
			GuiImage = AppGame.GuiRenderer.BindTexture(RenderTarget);
		}

		public void Resize(Vector2 size)
		{
			if (RenderTarget.Width != size.X || RenderTarget.Height != size.Y)
			{
				if (size.X > 1 && size.Y > 1)
				{

					AppGame.GuiRenderer.UnbindTexture(GuiImage);
					RenderTarget.Dispose();

					RenderTarget = new RenderTarget2D(AppGame.Device, (int)size.X, (int)size.Y);
					GuiImage = AppGame.GuiRenderer.BindTexture(RenderTarget);
				}
			}
		}




		public SpriteBatch BeginDraw()
		{
			AppGame.Device.SetRenderTarget(RenderTarget);
			AppGame.Device.Clear(Microsoft.Xna.Framework.Color.CornflowerBlue);

			return AppGame.Gui.SpriteBatch;
		}

		public SpriteBatch BeginDraw(Microsoft.Xna.Framework.Color clearColor)
		{
			AppGame.Device.SetRenderTarget(RenderTarget);
			AppGame.Device.Clear(clearColor);

			return AppGame.Gui.SpriteBatch;
		}

		public void EndDraw()
		{
			AppGame.Device.SetRenderTarget(AppGame.Gui.BackBuffer);
		}

		public void Dispose()
		{
			RenderTarget.Dispose();
			RenderTarget = null;
			GuiImage = AppGame.GuiRenderer.BindTexture(RenderTarget);
			GuiImage = IntPtr.Zero;
		}
	}
}
