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
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
      System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
      System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
      System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
      System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
      System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
      this.ProfileChart = new System.Windows.Forms.DataVisualization.Charting.Chart();
      this.ClassToPlot = new System.Windows.Forms.TextBox();
      this.ClassToPlotLabel = new System.Windows.Forms.Label();
      this.SelectionCriteriaPanel = new System.Windows.Forms.Panel();
      this.PlotLogDensity = new System.Windows.Forms.CheckBox();
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
      chartArea1.AlignmentOrientation = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal;
      chartArea1.AlignmentStyle = ((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles)((System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.PlotPosition | System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.AxesView)));
      chartArea1.AlignWithChartArea = "ChartArea2";
      chartArea1.AxisX.IsLabelAutoFit = false;
      chartArea1.AxisX.LabelAutoFitMaxFontSize = 12;
      chartArea1.AxisX.LabelStyle.Angle = 90;
      chartArea1.AxisX.LabelStyle.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
      chartArea1.AxisX.LabelStyle.Format = "#,##0.0";
      chartArea1.AxisX.MajorGrid.Enabled = false;
      chartArea1.AxisX.Title = "Abundance";
      chartArea1.AxisX2.LabelStyle.Format = "##0.00";
      chartArea1.AxisX2.MajorGrid.Enabled = false;
      chartArea1.AxisY.IsReversed = true;
      chartArea1.AxisY.IsStartedFromZero = false;
      chartArea1.AxisY.LabelAutoFitMaxFontSize = 12;
      chartArea1.AxisY.LabelStyle.Format = "##0";
      chartArea1.AxisY.MajorGrid.Enabled = false;
      chartArea1.AxisY.MajorTickMark.Interval = 0D;
      chartArea1.AxisY.MajorTickMark.IntervalOffset = 0D;
      chartArea1.AxisY.MajorTickMark.IntervalOffsetType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea1.AxisY.MajorTickMark.IntervalType = System.Windows.Forms.DataVisualization.Charting.DateTimeIntervalType.Number;
      chartArea1.AxisY.Title = "Depth 10 meters";
      chartArea1.AxisY.TitleFont = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      chartArea1.AxisY2.MajorGrid.Enabled = false;
      chartArea1.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea1.Name = "ChartArea1";
      chartArea1.Position.Auto = false;
      chartArea1.Position.Height = 85F;
      chartArea1.Position.Width = 45F;
      chartArea1.Position.Y = 10F;
      chartArea2.AlignmentOrientation = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentOrientations.Horizontal;
      chartArea2.AlignmentStyle = System.Windows.Forms.DataVisualization.Charting.AreaAlignmentStyles.AxesView;
      chartArea2.AlignWithChartArea = "ChartArea1";
      chartArea2.AxisX.IsLabelAutoFit = false;
      chartArea2.AxisX.LabelStyle.Angle = 90;
      chartArea2.AxisX.LabelStyle.Format = "#0.0";
      chartArea2.AxisX.MajorGrid.Enabled = false;
      chartArea2.AxisX.Title = "Volume Sampled";
      chartArea2.AxisY.IsReversed = true;
      chartArea2.AxisY.MajorGrid.Enabled = false;
      chartArea2.BorderDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
      chartArea2.Name = "ChartArea2";
      chartArea2.Position.Auto = false;
      chartArea2.Position.Height = 85F;
      chartArea2.Position.Width = 45F;
      chartArea2.Position.X = 48F;
      chartArea2.Position.Y = 10F;
      this.ProfileChart.ChartAreas.Add(chartArea1);
      this.ProfileChart.ChartAreas.Add(chartArea2);
      legend1.Alignment = System.Drawing.StringAlignment.Far;
      legend1.DockedToChartArea = "ChartArea1";
      legend1.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
      legend1.Name = "Legend1";
      legend2.Alignment = System.Drawing.StringAlignment.Far;
      legend2.DockedToChartArea = "ChartArea2";
      legend2.Docking = System.Windows.Forms.DataVisualization.Charting.Docking.Bottom;
      legend2.Name = "Legend2";
      this.ProfileChart.Legends.Add(legend1);
      this.ProfileChart.Legends.Add(legend2);
      this.ProfileChart.Location = new System.Drawing.Point(12, 100);
      this.ProfileChart.Name = "ProfileChart";
      series1.ChartArea = "ChartArea1";
      series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      series1.Legend = "Legend1";
      series1.Name = "Series1";
      series1.YValuesPerPoint = 2;
      series2.ChartArea = "ChartArea2";
      series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
      series2.Legend = "Legend1";
      series2.Name = "Series2";
      this.ProfileChart.Series.Add(series1);
      this.ProfileChart.Series.Add(series2);
      this.ProfileChart.Size = new System.Drawing.Size(822, 653);
      this.ProfileChart.TabIndex = 30;
      this.ProfileChart.Text = "Abundance/Deployment";
      title1.Name = "Title1";
      this.ProfileChart.Titles.Add(title1);
      this.ProfileChart.MouseClick += new System.Windows.Forms.MouseEventHandler(this.ProfileChart_MouseClick);
      // 
      // ClassToPlot
      // 
      this.ClassToPlot.Location = new System.Drawing.Point(182, 25);
      this.ClassToPlot.Name = "ClassToPlot";
      this.ClassToPlot.ReadOnly = true;
      this.ClassToPlot.Size = new System.Drawing.Size(328, 20);
      this.ClassToPlot.TabIndex = 52;
      // 
      // ClassToPlotLabel
      // 
      this.ClassToPlotLabel.AutoSize = true;
      this.ClassToPlotLabel.Location = new System.Drawing.Point(179, 9);
      this.ClassToPlotLabel.Name = "ClassToPlotLabel";
      this.ClassToPlotLabel.Size = new System.Drawing.Size(32, 13);
      this.ClassToPlotLabel.TabIndex = 54;
      this.ClassToPlotLabel.Text = "Class";
      // 
      // SelectionCriteriaPanel
      // 
      this.SelectionCriteriaPanel.BackColor = System.Drawing.Color.Silver;
      this.SelectionCriteriaPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.SelectionCriteriaPanel.Controls.Add(this.PlotLogDensity);
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
      this.SelectionCriteriaPanel.Size = new System.Drawing.Size(822, 82);
      this.SelectionCriteriaPanel.TabIndex = 57;
      // 
      // PlotLogDensity
      // 
      this.PlotLogDensity.AutoSize = true;
      this.PlotLogDensity.Location = new System.Drawing.Point(6, 56);
      this.PlotLogDensity.Name = "PlotLogDensity";
      this.PlotLogDensity.Size = new System.Drawing.Size(82, 17);
      this.PlotLogDensity.TabIndex = 74;
      this.PlotLogDensity.Text = "Log Density";
      this.PlotLogDensity.UseVisualStyleBackColor = true;
      this.PlotLogDensity.CheckedChanged += new System.EventHandler(this.PlotLogDensity_CheckedChanged);
      // 
      // CastField
      // 
      this.CastField.Location = new System.Drawing.Point(751, 25);
      this.CastField.Name = "CastField";
      this.CastField.ReadOnly = true;
      this.CastField.Size = new System.Drawing.Size(64, 20);
      this.CastField.TabIndex = 73;
      // 
      // CastLabel
      // 
      this.CastLabel.AutoSize = true;
      this.CastLabel.Location = new System.Drawing.Point(748, 9);
      this.CastLabel.Name = "CastLabel";
      this.CastLabel.Size = new System.Drawing.Size(28, 13);
      this.CastLabel.TabIndex = 72;
      this.CastLabel.Text = "Cast";
      // 
      // DeploymentField
      // 
      this.DeploymentField.Location = new System.Drawing.Point(3, 25);
      this.DeploymentField.Name = "DeploymentField";
      this.DeploymentField.ReadOnly = true;
      this.DeploymentField.Size = new System.Drawing.Size(173, 20);
      this.DeploymentField.TabIndex = 8;
      // 
      // SizeStaisticField
      // 
      this.SizeStaisticField.Location = new System.Drawing.Point(649, 25);
      this.SizeStaisticField.Name = "SizeStaisticField";
      this.SizeStaisticField.ReadOnly = true;
      this.SizeStaisticField.Size = new System.Drawing.Size(89, 20);
      this.SizeStaisticField.TabIndex = 71;
      // 
      // DeploymentLabel
      // 
      this.DeploymentLabel.AutoSize = true;
      this.DeploymentLabel.Location = new System.Drawing.Point(3, 9);
      this.DeploymentLabel.Name = "DeploymentLabel";
      this.DeploymentLabel.Size = new System.Drawing.Size(63, 13);
      this.DeploymentLabel.TabIndex = 3;
      this.DeploymentLabel.Text = "Deployment";
      // 
      // SizeStatiticLabel
      // 
      this.SizeStatiticLabel.AutoSize = true;
      this.SizeStatiticLabel.Location = new System.Drawing.Point(646, 9);
      this.SizeStatiticLabel.Name = "SizeStatiticLabel";
      this.SizeStatiticLabel.Size = new System.Drawing.Size(67, 13);
      this.SizeStatiticLabel.TabIndex = 66;
      this.SizeStatiticLabel.Text = "Size Statistic";
      // 
      // SizeRangeLabel
      // 
      this.SizeRangeLabel.AutoSize = true;
      this.SizeRangeLabel.Location = new System.Drawing.Point(520, 9);
      this.SizeRangeLabel.Name = "SizeRangeLabel";
      this.SizeRangeLabel.Size = new System.Drawing.Size(62, 13);
      this.SizeRangeLabel.TabIndex = 60;
      this.SizeRangeLabel.Text = "Size Range";
      // 
      // SizeRangeField
      // 
      this.SizeRangeField.Location = new System.Drawing.Point(522, 25);
      this.SizeRangeField.Name = "SizeRangeField";
      this.SizeRangeField.ReadOnly = true;
      this.SizeRangeField.Size = new System.Drawing.Size(115, 20);
      this.SizeRangeField.TabIndex = 68;
      // 
      // ChartSizeDepthDistribution
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(849, 762);
      this.Controls.Add(this.SelectionCriteriaPanel);
      this.Controls.Add(this.ProfileChart);
      this.MinimumSize = new System.Drawing.Size(865, 800);
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
    private System.Windows.Forms.CheckBox PlotLogDensity;


  }
}