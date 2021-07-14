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

        List<int>[] dataSamples;
        int         dataSampleMin = 0;
        int         dataSampleMax = 0;
        Color[]     dataSampleColor;
        HScrollBar  dataSampleScrollBar;

        public IReadOnlyList<int>[] Samples
        {
            get
            {
                return dataSamples;
            }
        }

        public uint SampleSpacing { get; private set; }

        public uint SampleChannelCount { get; private set; }
        
        public LineChart()
        {
            dataSamples = new List<int>[]
            {
                new List<int>()
            };

            dataSampleColor = new Color[]
            {
                ForeColor
            };

            SampleSpacing = 10;
            SampleChannelCount = 1;

            InitializeComponent();
            
            dataSampleScrollBar.Scroll += delegate (object _sender, ScrollEventArgs _e)
            {
                if (_e.NewValue != _e.OldValue)
                {
                    Refresh();
                }
            };
        }
        
        public void AddSample(uint channel, int value, bool refresh = true)
        {
            if (channel >= SampleChannelCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[channel].Add(value);

            UpdateDataSampleMinMax(false);
            UpdateScrollBarRange();

            if (refresh)
            {

                Refresh();
            }
        }
        public void AddSamples(uint channel, int[] values, bool refresh = true)
        {
            if (channel >= SampleChannelCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[channel].AddRange(values);

            UpdateDataSampleMinMax(false);
            UpdateScrollBarRange();

            if (refresh)
            {

                Refresh();
            }
        }

        public void ClearSamples(bool refresh = true)
        {
            for (uint i = 0; i < SampleChannelCount; ++i)
            {
                dataSamples[i].Clear();
            }
            
            ResetScrollBar(false);
            UpdateDataSampleMinMax(false);

            if (refresh)
            {

                Refresh();
            }
        }
        public void ClearSamples(uint channel, bool refresh = true)
        {
            if (channel >= SampleChannelCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[channel].Clear();

            UpdateDataSampleMinMax(false);

            if (refresh)
            {

                Refresh();
            }
        }

        public void SetSampleColor(uint channel, Color value, bool refresh = true)
        {
            if (channel >= SampleChannelCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSampleColor[channel] = value;

            if (refresh)
            {

                Refresh();
            }
        }

        public void SetSampleSpacing(uint value, bool refresh = true)
        {
            SampleSpacing = value;

            if (refresh)
            {

                Refresh();
            }
        }

        public void SetSampleChannelCount(uint value, bool refresh = true)
        {
            ClearSamples(
                refresh
            );

            dataSamples = new List<int>[value];
            dataSampleColor = new Color[value];

            for (int i = 0; i < dataSamples.Length; ++i)
            {
                dataSamples[i] = new List<int>();
                dataSampleColor[i] = ForeColor;
            }

            SampleChannelCount = value;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            var font = new Font(
                Font.FontFamily,
                6.0f
            );

            var refDataSampleColor = Color.FromArgb(
                70,
                ForeColor.R,
                ForeColor.G,
                ForeColor.B
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
        
        void ResetScrollBar(bool refresh)
        {
            dataSampleScrollBar.Value = 0;
            dataSampleScrollBar.Minimum = 0;
            dataSampleScrollBar.Maximum = 0;

            if (refresh)
            {

                Refresh();
            }
        }

        void UpdateScrollBarRange()
        {
            int dataSampleMaxCount = 0;

            foreach (var dataSampleList in dataSamples)
            {
                if (dataSampleList.Count > dataSampleMaxCount)
                {
                    dataSampleMaxCount = dataSampleList.Count;
                }
            }
            
            dataSampleScrollBar.Minimum = 0;
            dataSampleScrollBar.Maximum = dataSampleMaxCount;
        }

        void UpdateDataSampleMinMax(bool refresh)
        {
            dataSampleMin = 0;
            dataSampleMax = 0;

            foreach (var dataSampleList in dataSamples)
            {
                foreach (var dataSample in dataSampleList)
                {
                    if (dataSample < dataSampleMin)
                    {
                        dataSampleMin = dataSample;
                    }

                    if (dataSample > dataSampleMax)
                    {
                        dataSampleMax = dataSample;
                    }
                }
            }

            if (refresh)
            {

                Refresh();
            }
        }
        
        void OnPaint_Samples(PaintEventArgs e, Font font, Rectangle rectangle)
        {
            for (uint i = 0; i < SampleChannelCount; ++i)
            {
                var color = dataSampleColor[i];

                var pen = new Pen(
                    color
                );

                var brush = new SolidBrush(
                    color
                );

                OnPaint_Samples(
                    e,
                    pen,
                    brush,
                    font,
                    rectangle,
                    i
                );
            }
        }
        void OnPaint_Samples(PaintEventArgs e, Pen pen, Brush brush, Font font, Rectangle rectangle, uint index)
        {
            if (dataSamples[index].Count != 0)
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

                    return (int)(_rectangle.Bottom - y);
                }

                var sampleValue = dataSamples[index][sampleOffset];

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

                for (int i = (sampleOffset + 1), x = x_prev; (i < dataSamples[index].Count) && (x < rectangle.Right); ++i, x += (int)SampleSpacing)
                {
                    sampleValue = dataSamples[index][i];

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
