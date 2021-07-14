using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

namespace AL.DotNET.Forms.Controls
{
    public partial class LineChart
        : UserControl
    {
        List<int>[] dataSamples;
        int         dataSampleMin = 0;
        int         dataSampleMax = 0;
        Color[]     dataSampleColor;
        uint        dataSampleMaxCount = 0;
        HScrollBar  dataSampleScrollBar;

        public IReadOnlyList<int>[] Samples
        {
            get
            {
                return dataSamples;
            }
        }

        public uint SampleSpacing { get; private set; }

        public uint SampleGroupCount { get; private set; }
        
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

            SampleSpacing = 20;
            SampleGroupCount = 1;

            InitializeComponent();

            dataSampleScrollBar.ValueChanged += delegate (object _sender, EventArgs _e)
            {
                Refresh();
            };
        }
        
        public void AddSample(uint group, int value, bool refresh = true)
        {
            if (group >= SampleGroupCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[group].Add(value);

            UpdateDataSampleMinMax(false);
            UpdateScrollBarRange();

            if (refresh)
            {

                Refresh();
            }
        }
        public void AddSamples(uint group, int[] values, bool refresh = true)
        {
            if (group >= SampleGroupCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[group].AddRange(values);

            UpdateDataSampleMinMax(false);
            UpdateScrollBarRange();

            if (refresh)
            {

                Refresh();
            }
        }

        public void ClearSamples(bool refresh = true)
        {
            for (uint i = 0; i < SampleGroupCount; ++i)
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
        public void ClearSamples(uint group, bool refresh = true)
        {
            if (group >= SampleGroupCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSamples[group].Clear();

            UpdateDataSampleMinMax(false);

            if (refresh)
            {

                Refresh();
            }
        }

        public void SetSampleColor(uint group, Color value, bool refresh = true)
        {
            if (group >= SampleGroupCount)
            {

                throw new IndexOutOfRangeException();
            }

            dataSampleColor[group] = value;

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

        public void SetSampleGroupCount(uint value, bool refresh = true)
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

            SampleGroupCount = value;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            GetSampleRectangle(
                out Rectangle rectangle
            );
            
            OnPaint_Samples(
                e,
                rectangle
            );
        }
        
        void InitializeComponent()
        {
            SuspendLayout();

            Font = new Font(Font.FontFamily, 6.0f);
            ClientSize = new Size(400, 150);
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
            dataSampleScrollBar.Minimum = 0;
            dataSampleScrollBar.Maximum = (int)dataSampleMaxCount;
        }

        void UpdateDataSampleMinMax(bool refresh)
        {
            dataSampleMin = 0;
            dataSampleMax = 0;
            
            foreach (var dataSampleList in dataSamples)
            {
                if (dataSampleList.Count > dataSampleMaxCount)
                {
                    dataSampleMaxCount = (uint)dataSampleList.Count;
                }

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
        
        void OnPaint_Samples(PaintEventArgs e, Rectangle rectangle)
        {
            for (uint group = 0; group < SampleGroupCount; ++group)
            {
                var color = dataSampleColor[group];

                OnPaint_Samples(
                    e,
                    rectangle,
                    new Pen(color),
                    new SolidBrush(color),
                    group
                );
            }
        }
        void OnPaint_Samples(PaintEventArgs e, Rectangle rectangle, Pen pen, Brush brush, uint group)
        {
            var dataSamples = this.dataSamples[group];
            
            for (int i = dataSampleScrollBar.Value, x = rectangle.X, y = -1; i < dataSamples.Count; ++i)
            {
                if (!OnPaint_Sample(e, rectangle, pen, brush, dataSamples[i], dataSampleMin, dataSampleMax, ref x, ref y))
                {

                    break;
                }
            }
        }
        
        bool OnPaint_Sample(PaintEventArgs e, Rectangle rectangle, Pen pen, Brush brush, int value, int minValue, int maxValue, ref int x, ref int y)
        {
            if (x >= rectangle.Right)
            {

                return false;
            }

            if ((x + SampleSpacing) > rectangle.Right)
            {

                rectangle.Width -= (x + (int)SampleSpacing) - rectangle.Right;
            }

            int GetDataSamplePosition_Y(long _value, long _min, long _max, Rectangle _rectangle, float _fontHeight)
            {
                long delta = 0;

                if ((_min < 0) && (_max > 0))
                {
                    delta = -_min;

                    _min += delta;
                    _max += delta;

                    _value += delta;
                }

                return (int)(_rectangle.Bottom - (long)Math.Round(((_rectangle.Height - _fontHeight) / (double)(_max - _min)) * _value));
            }

            var fontHeight = Font.GetHeight();

            var _y = GetDataSamplePosition_Y(
                value,
                minValue,
                maxValue,
                rectangle,
                fontHeight
            );

            e.Graphics.DrawLine(
                pen,
                x,
                _y,
                x + (int)SampleSpacing,
                _y
            );

            var valueString = value.ToString();

            e.Graphics.DrawString(
                valueString,
                Font,
                brush,
                x + ((SampleSpacing / 2) - (e.Graphics.MeasureString(valueString, Font).Width / 2)),
                _y - fontHeight
            );

            if (y != -1)
            {
                e.Graphics.DrawLine(
                    pen,
                    x,
                    y,
                    x,
                    _y
                );
            }

            x += (int)SampleSpacing;
            y = _y;

            return true;
        }
    }
}
