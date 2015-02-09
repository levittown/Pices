namespace PicesCommander
{
  partial class ChartSizeDepthDistribution
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.components = new System.ComponentModel.Container();
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
      System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
      System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
      this.ProfileChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
      this.timer1 = new System.Windows.Forms.Timer(this.components);
      this.ClassToPlot = new System.Windows.Forms.TextBox();
      this.ClassToPlotLabel = new System.Windows.Forms.Label();
      this.SelectionCriteriaPanel = new System.Windows.Forms.Panel();
      this.CastField = new System.Windows.Forms.TextBox();
      this.CastLabel = new System.Windows.Forms.Label();
      this.DeploymentField = new System.Windows.Forms.TextBox();
      this.SizeStaisticField = new System.Windows.Forms.TextBox();
      this.DeploymentLabel = new System.Windows.Forms.Label();
      this.SizeStatiticLabel = new System.Windows.Forms.Label();
      this.SizeRangeLabel = new System.Windows.Forms.Label();
      this.SizeRangeField = new System.Windows.Forms.TextBox();
      ((System.ComponentModel.ISupportInitialize)(this.ProfileChart)).BeginInit();
      this.SelectionCriteriaPanel.SuspendLayout();
      this.SuspendLayout();
      // 
      // ProfileChart
      // 
      this.ProfileChart.BorderlineColor = System.Drawing.Color.Black;
      this.ProfileChart.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      this.ProfileChart.CausesValidation = false;
      chartArea2.AlignmentOrientation = ((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations)((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Vertical | System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal)));
      chartArea2.AlignWithChartArea = "ChartArea1";
      chartArea2.AxisX.LabelAutoFitMaxFontSize = 12;
      chartArea2.AxisX.LabelStyle.Format = "##0.000";
      chartArea2.AxisX.MajorGrid.Enabled = false;
      chartArea2.AxisX.Title = "Size mm^3";
      chartArea2.AxisX2.LabelStyle.Format = "##0.00";
      chartArea2.AxisX2.MajorGrid.Enabled = false;
      chartArea2.AxisY.IsStartedFromZero = false;
      chartArea2.AxisY.LabelAutoFitMaxFontSize = 12;
      chartArea2.AxisY.LabelStyle.Format = "#0.000";
      chartArea2.AxisY.MajorGrid.Enabled = false;
      chartArea2.AxisY.MajorTickMark.Interval = 0D;
      chartArea2.AxisY.MajorTickMark.IntervalOffset = 0D;
      chartArea2.AxisY.MajorTickMark.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea2.AxisY.MajorTickMark.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea2.AxisY.Title = "Log10 Denisty";
      chartArea2.AxisY2.MajorGrid.Enabled = false;
      chartArea2.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea2.Name = "ChartArea1";
      this.ProfileChart.ChartAreas.Add(chartArea2);
      legend2.Alignment = System.Drawing.StringAlignment.Far;
      legend2.DockedToChartArea = "ChartArea1";
      legend2.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Top;
      legend2.Name = "Legend1";
      this.ProfileChart.Legends.Add(legend2);
      this.ProfileChart.Location = new System.Drawing.Point(12, 90);
      this.ProfileChart.Name = "ProfileChart";
      series2.ChartArea = "ChartArea1";
      series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series2.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      series2.Legend = "Legend1";
      series2.Name = "Series1";
      this.ProfileChart.Series.Add(series2);
      this.ProfileChart.Size = new System.Drawing.Size(822, 518);
      this.ProfileChart.TabIndex = 30;
      this.ProfileChart.Text = "Abundance/Deployment";
      title2.Name = "Title1";
      this.ProfileChart.Titles.Add(title2);
      this.ProfileChart.MouseClick += new System.Windows.Forms.MouseEventHandler(this.ProfileChart_MouseClick);
      // 
      // timer1
      // 
      this.timer1.Interval = 50;
      this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
      // 
      // ClassToPlot
      // 
      this.ClassToPlot.Location = new System.Drawing.Point(182, 30);
      this.ClassToPlot.Name = "ClassToPlot";
      this.ClassToPlot.ReadOnly = true;
      this.ClassToPlot.Size = new System.Drawing.Size(328, 20);
      this.ClassToPlot.TabIndex = 52;
      // 
      // ClassToPlotLabel
      // 
      this.ClassToPlotLabel.AutoSize = true;
      this.ClassToPlotLabel.Location = new System.Drawing.Point(179, 14);
      this.ClassToPlotLabel.Name = "ClassToPlotLabel";
      this.ClassToPlotLabel.Size = new System.Drawing.Size(32, 13);
      this.ClassToPlotLabel.TabIndex = 54;
      this.ClassToPlotLabel.Text = "Class";
      // 
      // SelectionCriteriaPanel
      // 
      this.SelectionCriteriaPanel.BackColor = System.Drawing.Color.Silver;
      this.SelectionCriteriaPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.SelectionCriteriaPanel.Controls.Add(this.CastField);
      this.SelectionCriteriaPanel.Controls.Add(this.CastLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.DeploymentField);
      this.SelectionCriteriaPanel.Controls.Add(this.SizeStaisticField);
      this.SelectionCriteriaPanel.Controls.Add(this.DeploymentLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.SizeStatiticLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.ClassToPlotLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.ClassToPlot);
      this.SelectionCriteriaPanel.Controls.Add(this.SizeRangeLabel);
      this.SelectionCriteriaPanel.Controls.Add(this.SizeRangeField);
      this.SelectionCriteriaPanel.Location = new System.Drawing.Point(12, 12);
      this.SelectionCriteriaPanel.Name = "SelectionCriteriaPanel";
      this.SelectionCriteriaPanel.Size = new System.Drawing.Size(822, 72);
      this.SelectionCriteriaPanel.TabIndex = 57;
      // 
      // CastField
      // 
      this.CastField.Location = new System.Drawing.Point(751, 30);
      this.CastField.Name = "CastField";
      this.CastField.ReadOnly = true;
      this.CastField.Size = new System.Drawing.Size(64, 20);
      this.CastField.TabIndex = 73;
      // 
      // CastLabel
      // 
      this.CastLabel.AutoSize = true;
      this.CastLabel.Location = new System.Drawing.Point(748, 14);
      this.CastLabel.Name = "CastLabel";
      this.CastLabel.Size = new System.Drawing.Size(28, 13);
      this.CastLabel.TabIndex = 72;
      this.CastLabel.Text = "Cast";
      // 
      // DeploymentField
      // 
      this.DeploymentField.Location = new System.Drawing.Point(3, 30);
      this.DeploymentField.Name = "DeploymentField";
      this.DeploymentField.ReadOnly = true;
      this.DeploymentField.Size = new System.Drawing.Size(173, 20);
      this.DeploymentField.TabIndex = 8;
      // 
      // SizeStaisticField
      // 
      this.SizeStaisticField.Location = new System.Drawing.Point(649, 30);
      this.SizeStaisticField.Name = "SizeStaisticField";
      this.SizeStaisticField.ReadOnly = true;
      this.SizeStaisticField.Size = new System.Drawing.Size(89, 20);
      this.SizeStaisticField.TabIndex = 71;
      // 
      // DeploymentLabel
      // 
      this.DeploymentLabel.AutoSize = true;
      this.DeploymentLabel.Location = new System.Drawing.Point(3, 14);
      this.DeploymentLabel.Name = "DeploymentLabel";
      this.DeploymentLabel.Size = new System.Drawing.Size(63, 13);
      this.DeploymentLabel.TabIndex = 3;
      this.DeploymentLabel.Text = "Deployment";
      // 
      // SizeStatiticLabel
      // 
      this.SizeStatiticLabel.AutoSize = true;
      this.SizeStatiticLabel.Location = new System.Drawing.Point(646, 14);
      this.SizeStatiticLabel.Name = "SizeStatiticLabel";
      this.SizeStatiticLabel.Size = new System.Drawing.Size(67, 13);
      this.SizeStatiticLabel.TabIndex = 66;
      this.SizeStatiticLabel.Text = "Size Statistic";
      // 
      // SizeRangeLabel
      // 
      this.SizeRangeLabel.AutoSize = true;
      this.SizeRangeLabel.Location = new System.Drawing.Point(520, 14);
      this.SizeRangeLabel.Name = "SizeRangeLabel";
      this.SizeRangeLabel.Size = new System.Drawing.Size(62, 13);
      this.SizeRangeLabel.TabIndex = 60;
      this.SizeRangeLabel.Text = "Size Range";
      // 
      // SizeRangeField
      // 
      this.SizeRangeField.Location = new System.Drawing.Point(522, 30);
      this.SizeRangeField.Name = "SizeRangeField";
      this.SizeRangeField.ReadOnly = true;
      this.SizeRangeField.Size = new System.Drawing.Size(115, 20);
      this.SizeRangeField.TabIndex = 68;
      // 
      // ChartSizeDepthDistribution
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(849, 620);
      this.Controls.Add(this.SelectionCriteriaPanel);
      this.Controls.Add(this.ProfileChart);
      this.MinimumSize = new System.Drawing.Size(865, 658);
      this.Name = "ChartSizeDepthDistribution";
      this.Text = "Chart Size Depth Distribution";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ChartSizeDepthDistribution_FormClosing);
      this.Load += new System.EventHandler(this.ChartAbundanceByDeployment_Load);
      this.SizeChanged += new System.EventHandler(this.ChartAbundanceByDeployment_SizeChanged);
      this.Resize += new System.EventHandler(this.ChartAbundanceByDeployment_Resize);
      ((System.ComponentModel.ISupportInitialize)(this.ProfileChart)).EndInit();
      this.SelectionCriteriaPanel.ResumeLayout(false);
      this.SelectionCriteriaPanel.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.DataVisualization.Charting.Chart ProfileChart;
    private System.Windows.Forms.Timer timer1;
    private System.Windows.Forms.TextBox ClassToPlot;
    private System.Windows.Forms.Label ClassToPlotLabel;
    private System.Windows.Forms.Panel SelectionCriteriaPanel;
    private System.Windows.Forms.Label DeploymentLabel;
    private System.Windows.Forms.TextBox DeploymentField;
    private System.Windows.Forms.Label SizeRangeLabel;
    private System.Windows.Forms.Label SizeStatiticLabel;
    private System.Windows.Forms.TextBox CastField;
    private System.Windows.Forms.Label CastLabel;
    private System.Windows.Forms.TextBox SizeStaisticField;
    private System.Windows.Forms.TextBox SizeRangeField;


  }
}