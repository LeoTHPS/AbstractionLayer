using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AL.DotNET.Forms.Controls
{
    public partial class LineChart
        : UserControl
    {
        const int DATA_SAMPLE_REFERENCE_LINE_PADDING = 15;

        readonly List<int> dataSamples;
        int                dataSampleMin = 0;
        int                dataSampleMax = 0;
        Color              dataSampleColor;
        uint               dataSampleSpacing = 10;
        HScrollBar         dataSampleScrollBar;
        
        public IReadOnlyList<int> Samples
        {
            get
            {
                return dataSamples;
            }
        }

        public Color SampleColor
        {
            get
            {
                return dataSampleColor;
            }
            set
            {
                dataSampleColor = value;

                Refresh();
            }
        }

        public uint SampleSpacing
        {
            get
            {
                return dataSampleSpacing;
            }
            set
            {
                dataSampleSpacing = value;

                Refresh();
            }
        }

        public LineChart()
        {
            dataSampleColor = ForeColor;
            dataSamples = new List<int>();

            InitializeComponent();
            
            dataSampleScrollBar.Scroll += delegate (object _sender, ScrollEventArgs _e)
            {
                if (_e.NewValue != _e.OldValue)
                {
                    Refresh();
                }
            };
        }

        public void AddSample(int value)
        {
            dataSamples.Add(
                value
            );

            if (value < dataSampleMin)
            {

                dataSampleMin = value;
            }

            if (value > dataSampleMax)
            {

                dataSampleMax = value;
            }

            ++dataSampleScrollBar.Maximum;

            Refresh();
        }

        public void AddSamples(int[] values)
        {
            dataSamples.AddRange(
                values
            );

            foreach (var value in values)
            {
                if (value < dataSampleMin)
                {

                    dataSampleMin = value;
                }

                if (value > dataSampleMax)
                {

                    dataSampleMax = value;
                }
            }

            dataSampleScrollBar.Maximum += values.Length;

            Refresh();
        }

        public void ClearSamples()
        {
            dataSamples.Clear();
            dataSampleMin = 0;
            dataSampleMax = 0;

            dataSampleScrollBar.Value = 0;
            dataSampleScrollBar.Minimum = 0;
            dataSampleScrollBar.Maximum = 0;

            Refresh();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            var font = new Font(
                Font.FontFamily,
                6.0f
            );

            var refDataSampleColor = Color.FromArgb(
                70,
                SampleColor.R,
                SampleColor.G,
                SampleColor.B
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
                new Pen(refDataSampleColor),
                new SolidBrush(refDataSampleColor),
                font,
                rectangle,
                out Rectangle sampleContentRectangle
            );

            OnPaint_Samples(
                e,
                new Pen(SampleColor),
                new SolidBrush(SampleColor),
                font,
                sampleContentRectangle
            );

            base.OnPaint(e);
        }
        
        protected override void OnClientSizeChanged(EventArgs e)
        {
            base.OnClientSizeChanged(e);
            
            if (dataSampleScrollBar != null)
            {
                GetSampleRectangle(
                    out Rectangle sampleAreaRectangle
                );

                if ((sampleAreaRectangle.Height % DATA_SAMPLE_REFERENCE_LINE_PADDING) != 0)
                {
                    ClientSize = new Size(
                        ClientSize.Width,
                        (ClientSize.Height - sampleAreaRectangle.Height) + ((sampleAreaRectangle.Height / DATA_SAMPLE_REFERENCE_LINE_PADDING) * DATA_SAMPLE_REFERENCE_LINE_PADDING)
                    );
                }
            }
        }

        void InitializeComponent()
        {
            SuspendLayout();

            ClientSize = new Size(400, DATA_SAMPLE_REFERENCE_LINE_PADDING * 10);
            BorderStyle = BorderStyle.FixedSingle;

            InitializeComponent_DataSampleScrollBar();

            ResumeLayout(false);
        }

        void InitializeComponent_DataSampleScrollBar()
        {
            dataSampleScrollBar = new HScrollBar
            {
                Anchor = AnchorStyles.Left | AnchorStyles.Bottom | AnchorStyles.Right,

                Value = 0,
                Minimum = 0,
                Maximum = 0
            };

            dataSampleScrollBar.Size = new Size(ClientSize.Width, dataSampleScrollBar.Height);
            dataSampleScrollBar.Location = new Point(ClientRectangle.Left, ClientRectangle.Bottom - dataSampleScrollBar.Height);

            Controls.Add(dataSampleScrollBar);
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

            rectangle.Height -= dataSampleScrollBar.Height;
        }
        
        void OnPaint_Samples(PaintEventArgs e, Pen pen, Brush brush, Font font, Rectangle rectangle)
        {
            if (dataSamples.Count != 0)
            {
                var sampleOffset = dataSampleScrollBar.Value;

                int GetDataSamplePosition_Y(long _value, long _min, long _max, Rectangle _rectangle)
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

                var sampleValue = dataSamples[sampleOffset];

                int x_prev;

                var y_prev = GetDataSamplePosition_Y(
                    sampleValue,
                    dataSampleMin,
                    dataSampleMax,
                    rectangle
                );

                e.Graphics.DrawLine(
                    pen,
                    rectangle.Left,
                    y_prev,
                    x_prev = (rectangle.Left + (int)SampleSpacing),
                    y_prev
                );

                for (int i = (sampleOffset + 1), x = x_prev; (i < dataSamples.Count) && (x < rectangle.Right); ++i, x += (int)SampleSpacing)
                {
                    sampleValue = dataSamples[i];

                    var y = GetDataSamplePosition_Y(
                        sampleValue,
                        dataSampleMin,
                        dataSampleMax,
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
                        x_prev = (x + (int)SampleSpacing) < rectangle.Right ? (x + (int)SampleSpacing) : rectangle.Right,
                        y_prev = y
                    );
                }
            }
        }

        void OnPaint_References(PaintEventArgs e, Pen pen, Brush brush, Font font, Rectangle rectangle, out Rectangle contentRectangle)
        {
            if (dataSampleMin == dataSampleMax)
            {
                var dataSampleMetrics = e.Graphics.MeasureString(
                    dataSampleMin.ToString(),
                    font
                );

                contentRectangle = new Rectangle(
                   rectangle.X + (int)Math.Round(dataSampleMetrics.Width),
                   rectangle.Y,
                   rectangle.Width - (int)Math.Round(dataSampleMetrics.Width),
                   rectangle.Height
                );

                e.Graphics.DrawLine(
                    pen,
                    rectangle.Left + dataSampleMetrics.Width,
                    rectangle.Top + (rectangle.Height / 2),
                    rectangle.Right,
                    rectangle.Top + (rectangle.Height / 2)
                );

                e.Graphics.DrawString(
                    dataSampleMin.ToString(),
                    font,
                    brush,
                    rectangle.Left,
                    ((rectangle.Top + (rectangle.Height / 2)) - (font.GetHeight() / 2)) + 1
                );
            }
            else
            {
                var dataSampleMinMetrics = e.Graphics.MeasureString(
                    dataSampleMin.ToString(),
                    font
                );

                var dataSampleMaxMetrics = e.Graphics.MeasureString(
                    dataSampleMax.ToString(),
                    font
                );

                var dataSampleLargestMetrics = (dataSampleMaxMetrics.Width > dataSampleMinMetrics.Width) ? dataSampleMaxMetrics : dataSampleMinMetrics;

                contentRectangle = new Rectangle(
                   rectangle.X + (int)Math.Round(dataSampleLargestMetrics.Width),
                   rectangle.Y,
                   rectangle.Width - (int)Math.Round(dataSampleLargestMetrics.Width),
                   rectangle.Height
                );

                int dataSampleReferenceCount = (rectangle.Height / DATA_SAMPLE_REFERENCE_LINE_PADDING) - 1;
                
                for (int i = 0, y = rectangle.Bottom; i < dataSampleReferenceCount; ++i, y -= DATA_SAMPLE_REFERENCE_LINE_PADDING)
                {
                    e.Graphics.DrawLine(
                        pen,
                        contentRectangle.Left,
                        y - DATA_SAMPLE_REFERENCE_LINE_PADDING,
                        contentRectangle.Right,
                        y - DATA_SAMPLE_REFERENCE_LINE_PADDING
                    );
                    
                    e.Graphics.DrawString(
                        Math.Round(dataSampleMin + (((float)(dataSampleMax - dataSampleMin) / dataSampleReferenceCount) * (i + 1)), 0).ToString(),
                        font,
                        brush,
                        rectangle.Left,
                        ((y - DATA_SAMPLE_REFERENCE_LINE_PADDING) - (font.GetHeight() / 2)) + 1
                    );
                }
            }
        }
    }
}
