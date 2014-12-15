using System;
using System.Collections.Generic;
using System.Text;

using  PicesInterface;


/**
 *@brief  Cloned from "SizeDistribution" with the intent to use for Image Size in mm^2  a floating pont number.
 *@details The sizing will range by a factor with a starting value and growth rate.
 */

namespace PicesCommander
{

//#define  BucketCount  50
//#define  BucketSize   100


  public  class  SizeDistribution 
  {
    class  ClassTotals2  
    {
      int      bucketCount = 0;
      int      count       = 0;
      String   name        = null;
      String   nameUpper   = null;
      
      float    sizeInitial      = 0.5f; /**<  Starting size; all following buckest will grow by growth factor. */
      float    sizeGrowthFactor = 1.2f; 
      float    sizeEndRange     = 10.0f;
      float[]  sizeBucketStart  = null; /**<  the start of the size range for the respective bucket. */
      float[]  sizeBucketEnd    = null; /**<  the end of the size range for the respective bucket.   */
      int[]    sizeBuckets      = null;


      public  ClassTotals2 (String _name,
                            float  _sizeInitial,
                            float  _sizeGrowthFactor,
                            float  _sizeMaxSize
                          )
      {
        count            = 0;
        name             = _name;
        nameUpper        = _name.ToUpper ();
        sizeInitial      = _sizeInitial;
        sizeGrowthFactor = _sizeGrowthFactor;
        sizeEndRange     = _sizeEndRange;

        {
          // Compute num number of uckets needed.
          bucketCount = 0;
          ++bucketCount; // For images less than "sizeInitial".

          float sizeBegin = _sizeInitial;
          while  (sizeBegin <= _sizeMaxSize)
          {
            sizeBegin = sizeBegin * sizeGrowthFactor;
            ++bucketCount;
          }

          ++bucketCount;  // For images that are larger than "sizeEndRange".
        }

        sizeBuckets     = new int[bucketCount];
        sizeBucketStart = new float[bucketCount];
        sizeBucketEnd   = new float[bucketCount];

        {
          // Initialize Bucket arrays.
          int x = 0;
          sizeBucketStart[x] = 0.0f;
          sizeBucketEnd  [x] = sizeInitial;
          sizeBuckets    [x] = 0;
          ++x;

          float sizeBegin = _sizeInitial;
          while  (sizeBegin <= _sizeMaxSize)
          {
            float  sizeEndOfRange = sizeBegin * sizeGrowthFactor;
            sizeBucketStart[x] = sizeBegin;
            sizeBucketEnd  [x] = sizeEndOfRange;
            sizeBuckets    [x] = 0;
            ++x;
            sizeBegin = sizeEndOfRange;
          }

          sizeBucketStart[x] = sizeBegin;
          sizeBucketEnd  [x] = 99999.9999f;;
          sizeBuckets    [x] = 0;
          ++x;
        }
        count = 0;
      }
        

      public  String  Name       {get  {return name;}}

      public  String  NameUpper  {get  {return  nameUpper;}}


      public  void  Increment (float  areaMM)
      {
        int  bucket = -1;
        int  left  = 0;
        int  right = sizeBucketStart.Length - 1;

        while  (bucket < 0)
        {
          int  m = (left + right) / 2;
          if  (areaMM < sizeBucketStart[m])
            right = m - 1;
          else if  (areaMM >= sizeBucketEnd[m])
            left = m + 1;
          else
            bucket = m;
        }

        if  (bucket >= bucketCount)
          bucket = bucketCount - 1;

        if  (bucket < 0)
          bucket = 0;

        sizeBuckets[bucket]++;
        count++;
      }



      public  void  AddIn (ClassTotals2  classTotals)
      {
        int  idx;

        count = count + classTotals.count;

        for  (idx = 0; idx < bucketCount; idx++)
          sizeBuckets[idx] = sizeBuckets[idx] + classTotals.sizeBuckets[idx];
      }  /* AddIn */



      public  int   BucketCount (int idx)
      {
        if  ((idx < 0)  &&  (idx >= bucketCount))
          return 0;
        return  sizeBuckets[idx];
      }


      public  void  PrintFormatedLine (System.IO.StreamWriter  o)
      {
        String  s = name.PadRight (20)   ;
        o.Write (s);

        s = count.ToString ().PadLeft (9);
        o.Write (s);

        int  bucket;

        for (bucket = 0;  bucket < bucketCount;  bucket++)
        {
          s = sizeBuckets [bucket].ToString ().PadLeft (8);
          o.Write (s);
        }
        o.WriteLine ();
      }  /* PrintFormatedLine */



      public  void  PrintCSVLine (System.IO.StreamWriter  o)
      {
        o.Write ("\"" + name + "\"," + count.ToString ());

        int  bucket;

        for (bucket = 0;  bucket <  bucketCount; bucket++)
        {
          o.Write ("," + sizeBuckets[bucket].ToString ());
        }
        o.WriteLine ();
      }  /* PrintCSVLine */




      public  void  PrintTabDelLine (System.IO.StreamWriter  o)
      {
        o.Write ("\"" + name + "\"" + "\t" + count.ToString ());

        int  bucket;

        for (bucket = 0;  bucket <  bucketCount; bucket++)
          o.Write ("\t" + sizeBuckets[bucket].ToString ());

        o.WriteLine ();
      }
    };  /* PrintTabDelLine */




    class  ClassTotals2List:  List<ClassTotals2>
    {
      public  ClassTotals2List ()
      {
      }
      

      public  void  CleanUpMemory ()
      {
        foreach  (ClassTotals2 ct in this)
        {
          ct.CleaUpMemory ();
        }
        Clear ();
      }
      
   
      public  ClassTotals2  LookUp (String  _name)
      {
        _name = _name.ToUpper ();

        int  idx;
        ClassTotals2  classTotals = null;
        ClassTotals2  temp        = null;

        for  (idx = 0; (idx < Count)  &&  (classTotals == null);  idx++)
        {
          temp = (this)[idx];
          if  (temp.NameUpper == _name)
            classTotals = temp;
        }

        return  classTotals;
      }  /* LookUp */



      private class  ClassTotals2Comparator:  IComparer<ClassTotals2>
      {
        int  IComparer<ClassTotals2>.Compare (ClassTotals2 x, ClassTotals2 y)  
        {
          return x.Name.CompareTo (y.Name);
        }
      }  /* ClassTotals2Comparator */



      public  void  SortByName ()
      {
        Sort (new ClassTotals2Comparator ());
      }

    };  /* ClassTotals2List */

      
      
      

    
    float    sizeInitial      = 0.5f; /**<  Starting size; all following buckest will grow by growth factor. */
    float    sizeGrowthFactor = 1.2f; 
    float    sizeEndRange     = 10.0f;
    float[]  sizeBucketStart  = null; /**<  the start of the size range for the respective bucket. */
    float[]  sizeBucketEnd    = null; /**<  the end of the size range for the respective bucket.   */
    int[]    sizeBuckets      = null;
    
    int               bucketCount = 0;
    int               bucketSize  = 0;
    ClassTotals2List  totals      = null;



    public  SizeDistribution (float  _sizeInitial,
                              float  _sizeGrowthFactor,
                              float  _sizeMaxSize
                             )
    {
      sizeInitial      = _sizeInitial;
      sizeGrowthFactor = _sizeGrowthFactor;
      sizeEndRange     = _sizeEndRange;
      totals = new ClassTotals2List ();
      if  (bucketCount < 1)
        throw new Exception ("SizeDistribution     *** Invalid Bucket Size[" + bucketSize.ToString () + "] ***");
    }

  
    public  void  CleanUpMemory ()
    {
      if  (totals != null)
       totals.CleanUpMemory ();

      totals = null;
    }


    /// <summary>
    /// Will add in the contents of another SizeDistribution instance into this instance.
    /// </summary>
    /// <param name="x">The other 'SizeDistribution' instance to be acumulated to this instance.</param>
    public  void  Add (SizeDistribution  x)
    {
      if  ((x.bucketCount != this.bucketCount)  ||  (x.bucketSize != this.bucketSize))
      {
        throw new Exception ("'SizeDistribution.Add   ***ERROR***   Dimensions are not the same" + "\n" +
                             "Left.bucketCount[" + bucketCount.ToString () + "]  Right[" + x.bucketCount.ToString () + "]" + "\n" +
                             "Left.bucketSize [" +this.bucketSize.ToString () + "]  Right.bucketSize[" + x.bucketSize.ToString () + "]"
                             );
      }

      foreach (ClassTotals2  ct in x.totals)
      {
        ClassTotals2  classTotals = totals.LookUp (ct.Name);
        if  (classTotals == null)
        {
          classTotals = new ClassTotals2 (ct.Name, bucketCount, bucketSize);
          totals.Add (classTotals);
        }    
        classTotals.AddIn (ct);
      }
    }  /* SizeDistribution */



    /// <summary>
    /// Method used to giurantee that the specified class is included; even if none occur.
    /// </summary>
    /// <param name="mlClass">Class to add </param>
    public void   InitiateClass (PicesClass  mlClass)
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals2  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals2 (mlClass.Name, bucketCount, bucketSize);
        totals.Add (classTotals);
      }    
      return;
    }  /* InitiateClass */




    public void   Increment (PicesClass  mlClass,
                             int         size
                            )
    {
      if  (mlClass == null)
        mlClass = PicesClassList.GetUnKnownClassStatic ();

      ClassTotals2  classTotals = totals.LookUp (mlClass.Name);
      if  (classTotals == null)
      {
        classTotals = new ClassTotals2 (mlClass.Name, bucketCount, bucketSize);
        totals.Add (classTotals);
      }    

      classTotals.Increment (size);
    }  /* Increment */

    
 
    public  void     PrintFormatedDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintFormatedHeader (o);

      ClassTotals2  classTotals = null;

      ClassTotals2  grandTotals = new ClassTotals2 ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count;  idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintFormatedLine (o);
        grandTotals.AddIn (classTotals);
      }

      o.WriteLine ();
      grandTotals.PrintFormatedLine (o);
    }  /* PrintFormatedDistributionMatrix */



    public  void     PrintCSVDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintCSVHeader (o);

      ClassTotals2  classTotals = null;

      ClassTotals2  grandTotals = new ClassTotals2 ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count; idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintCSVLine (o);
        grandTotals.AddIn (classTotals);
      }

      o.WriteLine ();
      grandTotals.PrintCSVLine (o);
    }  /* PrintCSVDistributionMatrix */





    public  void   PrintTabDelDistributionMatrix (System.IO.StreamWriter  o)
    {
      totals.SortByName ();

      PrintTabDelHeader (o);

      ClassTotals2  classTotals = null;

      ClassTotals2  grandTotals = new ClassTotals2 ("Grand Totals", bucketCount, bucketSize);

      int  idx;

      for  (idx = 0;  idx < totals.Count;  idx++)
      {
        classTotals = totals[idx];
        classTotals.PrintTabDelLine (o);
        grandTotals.AddIn (classTotals);
      }   
      o.WriteLine ();
      grandTotals.PrintTabDelLine (o);
    }  /* PrintTabDelDistributionMatrix */




    public void  PrintByClassCollumns (System.IO.StreamWriter        o,
                                       List<uint>                    scanLinesPerMeterDepth,
                                       List<InstrumentStatsByDepth>  volumePerMeterDepth,
                                       bool                          printDensity            /**< Indicates to print density rather than counts. */
                                      )  

    {
      PicesClassList  classes = BuildMLClassList ();
      int  cIDX;
 
      // Find the first and last buckets with activity
  
      int  firstBucket = -1;
      int  lastBucket = 0;
  
      for  (int bucketIDX = 0;  bucketIDX < bucketCount;  bucketIDX++)
      {
        int  bucketTotal = 0;
        for  (cIDX = 0;  cIDX < classes.Count;  cIDX++)
        {
          PicesClass   mlClass = classes[cIDX];
          ClassTotals2  classTotals = totals.LookUp (mlClass.Name);
          bucketTotal += classTotals.BucketCount (bucketIDX);
        }

        if  (bucketTotal > 0)
        {
          if  (firstBucket < 0)
             firstBucket = bucketIDX;
          lastBucket = bucketIDX;
        }
	    }

      if  (firstBucket < 0)
      {
        o.WriteLine ();
        o.WriteLine ();
        o.WriteLine ("SizeDistribution::PrintByClassCollumns     *** There is no SizeDistribution Data ***");
        o.WriteLine ();
        return;
      }

      int[]  finalTotals = new int[classes.Count];
      for  (int zed = 0;  zed < classes.Count;  zed++)
        finalTotals[zed] = 0;

      int  grandTotal = 0;

      List<String> headLines = classes.ExtractThreeTitleLines ();
      while  (headLines.Count < 3)
        headLines.Add ("");

      String s1 = "Abundance by Class";
      if  (printDensity)
        s1 = "Abundance/m-3";
      
      String  temperatureUOM      = PicesInstrumentData.TemperatureUnit;
      String  fluorescenceUOM     = PicesInstrumentData.FluorescenceUnit;
      String  densityUOM          = PicesInstrumentData.DensityUnit;
      String  oxygenUOM           = PicesInstrumentData.OxygenUnit;
      String  salinityUOM         = PicesInstrumentData.SalinityUnit;
      String  transmisivityUMO    = PicesInstrumentData.TransmisivityUnit;
      String  turbidityUMO        = PicesInstrumentData.TurbidityUnit;
      String  cdomFluorescenceUMO = PicesInstrumentData.CdomFluorescenceUnit;


      //   [O2 (ml/L) * 44.64]/1.027 = O2 umol/kg 

      o.WriteLine (""         + "\t" + ""      + "\t" + ""       + "\t" + s1);
      o.WriteLine (""         + "\t" + ""      + "\t" + ""       + "\t" + headLines[0] + "\t" + s1);
      o.WriteLine (""         + "\t" + "Scan"  + "\t" + "Volume" + "\t" + headLines[1] + "\t" + "All"     + "\t" + "Temperature"   + "\t" + "Salinity"  + "\t" + "Density"   + "\t" + "Fluorescence"  + "\t" + "FluorescenceSensor"  + "\t" + "Oxygen"  + "\t" + "Oxygen"   + "\t" + "Transmisivity"   + "\t" + "Turbidity"  + "\t" + "CdomFluorescence");
      o.WriteLine ("Depth(m)" + "\t" + "Lines" + "\t" + "m-3"    + "\t" + headLines[2] + "\t" + "Classes" + "\t" + temperatureUOM  + "\t" + salinityUOM + "\t" + densityUOM  + "\t" + fluorescenceUOM + "\t" + "Volts"               + "\t" + oxygenUOM + "\t" + "umol/kg"  + "\t" + transmisivityUMO  + "\t" + turbidityUMO + "\t" + cdomFluorescenceUMO);

      ulong  totalScanLines = 0;
      double totalVolume    = 0.0f;

      int    imageSize = firstBucket * bucketSize;

      for  (int bucketIDX = firstBucket;  bucketIDX <= lastBucket;  bucketIDX++)
      {
        int  nextImageSize = imageSize + bucketSize;

        ulong  scanLinesDepthForThisBucket = 0;
        if  (scanLinesPerMeterDepth != null)
        {
          for  (int x = imageSize;  x < Math.Min (nextImageSize, scanLinesPerMeterDepth.Count);  x++)
            scanLinesDepthForThisBucket += scanLinesPerMeterDepth[x];
        }

        InstrumentStatsByDepth  totalThisBucketIdx = new InstrumentStatsByDepth (bucketIDX);
        double  volumeDepthForThisBucket = 0.0;
        if  (volumePerMeterDepth != null)
        {
          if  (bucketIDX < volumePerMeterDepth.Count)
            totalThisBucketIdx.Add (volumePerMeterDepth[bucketIDX]);
          for  (int x = imageSize;  x < Math.Min (nextImageSize, volumePerMeterDepth.Count);  x++)
          {
            if  (x < volumePerMeterDepth.Count)
              volumeDepthForThisBucket += volumePerMeterDepth[x].volumeSampled;
          }
        }
        totalThisBucketIdx.ComputeMean ();

        o.Write (imageSize.ToString ()                   + "\t" + 
                 scanLinesDepthForThisBucket.ToString () + "\t" + 
                 volumeDepthForThisBucket.ToString ()
                );

        totalScanLines += scanLinesDepthForThisBucket;
        totalVolume    += volumeDepthForThisBucket;

        int  bucketTotal = 0;

        int  intIDX = 0;
        for  (cIDX = 0;  cIDX < classes.Count;  cIDX++)
        {
          PicesClass   mlClass  = classes[cIDX];
          ClassTotals2  classTotals = totals.LookUp (mlClass.Name);

          int  qtyThisBucket = classTotals.BucketCount (bucketIDX);
          double  densityThisBucket = 0.0;
          if  (volumeDepthForThisBucket != 0.0)
            densityThisBucket = classTotals.BucketCount (bucketIDX) / volumeDepthForThisBucket;

          if  (printDensity)
            o.Write ("\t" + densityThisBucket.ToString ());
          else
            o.Write ("\t" + qtyThisBucket.ToString ());

          bucketTotal          += qtyThisBucket;
          finalTotals[intIDX]  += qtyThisBucket;
          grandTotal           += qtyThisBucket;

          intIDX++;
        }

        o.Write ("\t" + bucketTotal);

        if  (totalThisBucketIdx != null)
        {
          o.Write ("\t" + totalThisBucketIdx.temperatureMean        +
                   "\t" + totalThisBucketIdx.salinityMean           +
                   "\t" + totalThisBucketIdx.densityMean            +
                   "\t" + totalThisBucketIdx.fluorescenceMean       +
                   "\t" + totalThisBucketIdx.fluorescenceSensorMean +
                   "\t" + totalThisBucketIdx.oxygenMean             +
                   "\t" + totalThisBucketIdx.Oxygen_molPerKg        +
                   "\t" + totalThisBucketIdx.transmisivityMean      +
                   "\t" + totalThisBucketIdx.turbidityMean          +
                   "\t" + totalThisBucketIdx.cdomFluorescenceMean
                  );
        }

        imageSize = nextImageSize;
        o.WriteLine ();
	    }

      {
        o.WriteLine ();
        o.Write ("FinalTotals" + "\t"  + totalScanLines + "\t" + totalVolume);
        for  (uint x = 0;  x < classes.Count;  x++)
        {
          double  density = 0;
          if  (totalVolume != 0.0)
            density  = finalTotals[x] / totalVolume;

          if  (printDensity)
            o.Write ("\t" + "");
          else
            o.Write ("\t" + finalTotals[x]);
        }

        double  grandTotalDensity = 0.0;
        if  (totalVolume != 0.0)
          grandTotalDensity = grandTotal / totalVolume;

        if  (printDensity)
          o.Write ("\t" + grandTotalDensity.ToString ());
        else
          o.Write ("\t" + grandTotal.ToString ());
        o.WriteLine ();
      }

      classes = null;
    }  /* ReportByClassCollumns */



  

    private  PicesClassList  BuildMLClassList ()
    {
      PicesClassList  mlClasses = new PicesClassList ();

      int  cIDX;
      //for  (cIDX = 0;  cIDX <  totals->end ();  cIDX++)
      for  (cIDX = 0;  cIDX <  totals.Count;  cIDX++)
      {
        ClassTotals2  ct = totals[cIDX];
        mlClasses.Add (PicesClassList.GetUniqueClass (ct.Name, ""));
      }

      mlClasses.SortByName ();

      return  mlClasses;
    }  /* BuildMLClassList */



    void  PrintCSVHeader (System.IO.StreamWriter  o)
    {
       o.Write ("\"Class Name\",Sum,");

       int  imageSize = 0;
       int  bucket;

       for (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
       {
          imageSize = imageSize + bucketSize;
          o.Write ("," + imageSize.ToString ());
       }

       o.Write (",>" + imageSize.ToString ());
       o.WriteLine ();
    }  /* PrintCSVHeader */



    
    
    private  void  PrintFormatedHeader (System.IO.StreamWriter  o)
    {
      o.Write ("Class Name              TOTAL");
      int  imageSize = 0;
      int  bucket;

      for  (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
      {
        imageSize = imageSize + bucketSize;
        String  sizeStr;
        sizeStr = imageSize.ToString ().PadLeft (8);
        o.Write (sizeStr);
      }

      String  s = ">" + imageSize.ToString ();
      s = s.PadLeft (8);
      o.Write (s);
      o.WriteLine ();

      o.Write ("==================      =====");

      for (bucket = 0;  bucket <  bucketCount; bucket++)
      {
         o.Write ("    ====");
      }
      o.WriteLine ();
    }  /* PrintFormatedHeader */



    private  void  PrintTabDelHeader (System.IO.StreamWriter  o)
    {
      o.Write ("\"Class Name\"" + "\t" + "Sum");

      int  imageSize = 0;
      int  bucket;

      for (bucket = 0;  bucket <  (bucketCount - 1); bucket++)
      {
         imageSize = imageSize + bucketSize;
         o.Write ("\t" + imageSize.ToString ());
      }

      o.Write ("\t" + ">" + imageSize.ToString ());

      o.WriteLine ();
    }  /* PrintTabDelHeader */
  }  /* SizeDistribution */
}

