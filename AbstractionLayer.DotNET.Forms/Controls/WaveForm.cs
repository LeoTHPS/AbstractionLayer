using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AL.DotNET.Forms.Controls
{
    public partial class WaveForm
        : UserControl
    {
        const int WAVE_SAMPLE_REFERENCE_LINE_PADDING = 15;

        Color              waveColor;
        readonly List<int> waveSamples;
        int                waveSampleMin = 0;
        int                waveSampleMax = 0;
        uint               waveSampleSpacing = 10;
        HScrollBar         waveScrollBar;
        
        public Color WaveColor
        {
            get
            {
                return waveColor;
            }
            set
            {
                waveColor = value;

                Refresh();
            }
        }
        
        public IReadOnlyList<int> WaveSamples
        {
            get
            {
                return waveSamples;
            }
        }
        
        public uint WaveSampleSpacing
        {
            get
            {
                return waveSampleSpacing;
            }
            set
            {
                waveSampleSpacing = value;

                Refresh();
            }
        }

        public WaveForm()
        {
            waveColor = ForeColor;
            waveSamples = new List<int>();

            InitializeComponent();
            
            waveScrollBar.Scroll += delegate (object _sender, ScrollEventArgs _e)
            {
                if (_e.NewValue != _e.OldValue)
                {
                    Refresh();
                }
            };
        }

        public void AddWaveSample(int value)
        {
            waveSamples.Add(
                value
            );

            if (value < waveSampleMin)
            {

                waveSampleMin = value;
            }

            if (value > waveSampleMax)
            {

                waveSampleMax = value;
            }

            ++waveScrollBar.Maximum;

            Refresh();
        }

        public void AddWaveSampleRange(int[] values)
        {
            waveSamples.AddRange(
                values
            );

            foreach (var value in values)
            {
                if (value < waveSampleMin)
                {

                    waveSampleMin = value;
                }

                if (value > waveSampleMax)
                {

                    waveSampleMax = value;
                }
            }

            waveScrollBar.Maximum += values.Length;

            Refresh();
        }

        public void ClearWaveSamples()
        {
            waveSamples.Clear();
            waveSampleMin = 0;
            waveSampleMax = 0;

            waveScrollBar.Value = 0;
            waveScrollBar.Minimum = 0;
            waveScrollBar.Maximum = 0;

            Refresh();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            var font = new Font(
                Font.FontFamily,
                6.0f
            );

            var refWaveColor = Color.FromArgb(
                70,
                WaveColor.R,
                WaveColor.G,
                WaveColor.B
            );

            GetSampleRectangle(
                out Rectangle rectangle
            );

//            e.Graphics.DrawRectangle(
//                new Pen(ForeColor),
//                rectangle
//            );
            
            OnPaint_References(
                e,
                new Pen(refWaveColor),
                new SolidBrush(refWaveColor),
                font,
                rectangle,
                out Rectangle sampleContentRectangle
            );

            OnPaint_Samples(
                e,
                new Pen(WaveColor),
                new SolidBrush(WaveColor),
                font,
                sampleContentRectangle
            );

            base.OnPaint(e);
        }
        
        protected override void OnClientSizeChanged(EventArgs e)
        {
            base.OnClientSizeChanged(e);
            
            if (waveScrollBar != null)
            {
                GetSampleRectangle(
                    out Rectangle sampleAreaRectangle
                );

                if ((sampleAreaRectangle.Height % WAVE_SAMPLE_REFERENCE_LINE_PADDING) != 0)
                {
                    ClientSize = new Size(
                        ClientSize.Width,
                        (ClientSize.Height - sampleAreaRectangle.Height) + ((sampleAreaRectangle.Height / WAVE_SAMPLE_REFERENCE_LINE_PADDING) * WAVE_SAMPLE_REFERENCE_LINE_PADDING)
                    );
                }
            }
        }

        void InitializeComponent()
        {
            SuspendLayout();

            ClientSize = new Size(400, WAVE_SAMPLE_REFERENCE_LINE_PADDING * 10);
            BorderStyle = BorderStyle.FixedSingle;

            InitializeComponent_WaveScrollBar();

            ResumeLayout(false);
        }

        void InitializeComponent_WaveScrollBar()
        {
            waveScrollBar = new HScrollBar
            {
                Anchor = AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Right,

                Value = 0,
                Minimum = 0,
                Maximum = 0
            };

            waveScrollBar.Size = new Size(ClientSize.Width, waveScrollBar.Height);
            waveScrollBar.Location = new Point(ClientRectangle.Left, ClientRectangle.Bottom - waveScrollBar.Height);

            Controls.Add(waveScrollBar);
        }

        void GetSampleRectangle(out Rectangle rectangle)
        {
            rectangle = ClientRectangle;
            rectangle.X += Margin.Left;
            rectangle.Width -= Margin.Left;
            rectangle.Width -= Margin.Right;
            rectangle.Y += Margin.Top;
            rectangle.Height -= Margin.Top;
            rectangle.Height -= Margin.Bottom;

            rectangle.Height -= waveScrollBar.Height;
        }
        
        void OnPaint_Samples(PaintEventArgs e, Pen pen, Brush brush, Font font, Rectangle rectangle)
        {
            if (waveSamples.Count != 0)
            {
                var sampleOffset = waveScrollBar.Value;

                int GetWaveSamplePosition_Y(long _value, long _min, long _max, Rectangle _rectangle)
                {
                    long delta = 0;

                    if ((_min < 0) && (_max > 0))
                    {
                        delta = -_min;

                        _min += delta;
                        _max += delta;

                        _value += delta;
                    }

                    long range = _max - _min;

                    var y = (long)Math.Round(
                        (_rectangle.Height / (double)range) * _value
                    );

                    return (int)(_rectangle.Top + y);
                }

                var sampleValue = waveSamples[sampleOffset];

                int x_prev;

                var y_prev = GetWaveSamplePosition_Y(
                    sampleValue,
                    waveSampleMin,
                    waveSampleMax,
                    rectangle
                );

                e.Graphics.DrawLine(
                    pen,
                    rectangle.Left,
                    y_prev,
                    x_prev = (rectangle.Left + (int)WaveSampleSpacing),
                    y_prev
                );

                for (int i = (sampleOffset + 1), x = x_prev; (i < waveSamples.Count) && (x < rectangle.Right); ++i, x += (int)WaveSampleSpacing)
                {
                    sampleValue = waveSamples[i];

                    var y = GetWaveSamplePosition_Y(
                        sampleValue,
                        waveSampleMin,
                        waveSampleMax,
                        rectangle
                    );

                    e.Graphics.DrawLine(
                        pen,
                        x_prev,
                        y_prev,
                        x,
                        y
                    );

                    e.Graphics.DrawLine(
                        pen,
                        x,
                        y,
                        x_prev = (x + (int)WaveSampleSpacing) < rectangle.Right ? (x + (int)WaveSampleSpacing) : rectangle.Right,
                        y_prev = y
                    );
                }
            }
        }

        void OnPaint_References(PaintEventArgs e, Pen pen, Brush brush, Font font, Rectangle rectangle, out Rectangle contentRectangle)
        {
            if (waveSampleMin == waveSampleMax)
            {
                var waveSampleMetrics = e.Graphics.MeasureString(
                    waveSampleMin.ToString(),
                    font
                );

                contentRectangle = new Rectangle(
                   rectangle.X + (int)Math.Round(waveSampleMetrics.Width),
                   rectangle.Y,
                   rectangle.Width - (int)Math.Round(waveSampleMetrics.Width),
                   rectangle.Height
                );

                e.Graphics.DrawLine(
                    pen,
                    rectangle.Left + waveSampleMetrics.Width,
                    rectangle.Top + (rectangle.Height / 2),
                    rectangle.Right,
                    rectangle.Top + (rectangle.Height / 2)
                );

                e.Graphics.DrawString(
                    waveSampleMin.ToString(),
                    font,
                    brush,
                    rectangle.Left,
                    ((rectangle.Top + (rectangle.Height / 2)) - (font.GetHeight() / 2)) + 1
                );
            }
            else
            {
                var waveSampleMinMetrics = e.Graphics.MeasureString(
                    waveSampleMin.ToString(),
                    font
                );

                var waveSampleMaxMetrics = e.Graphics.MeasureString(
                    waveSampleMax.ToString(),
                    font
                );

                var waveSampleLargestMetrics = (waveSampleMaxMetrics.Width > waveSampleMinMetrics.Width) ? waveSampleMaxMetrics : waveSampleMinMetrics;

                contentRectangle = new Rectangle(
                   rectangle.X + (int)Math.Round(waveSampleLargestMetrics.Width),
                   rectangle.Y,
                   rectangle.Width - (int)Math.Round(waveSampleLargestMetrics.Width),
                   rectangle.Height
                );

                int waveSampleReferenceCount = (rectangle.Height / WAVE_SAMPLE_REFERENCE_LINE_PADDING) - 1;
                
                for (int i = 0, y = rectangle.Bottom; i < waveSampleReferenceCount; ++i, y -= WAVE_SAMPLE_REFERENCE_LINE_PADDING)
                {
                    e.Graphics.DrawLine(
                        pen,
                        contentRectangle.Left,
                        y - WAVE_SAMPLE_REFERENCE_LINE_PADDING,
                        contentRectangle.Right,
                        y - WAVE_SAMPLE_REFERENCE_LINE_PADDING
                    );
                    
                    e.Graphics.DrawString(
                        Math.Round(waveSampleMin + (((float)(waveSampleMax - waveSampleMin) / waveSampleReferenceCount) * (i + 1)), 0).ToString(),
                        font,
                        brush,
                        rectangle.Left,
                        ((y - WAVE_SAMPLE_REFERENCE_LINE_PADDING) - (font.GetHeight() / 2)) + 1
                    );
                }
            }
        }
    }
}
