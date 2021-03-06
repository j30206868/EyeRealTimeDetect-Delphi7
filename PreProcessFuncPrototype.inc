  procedure convolution(tarImg, buffImg: TImage; kernel: TInteger2DArray; kW, kH: Integer; bpp: Integer; ratio: double);
  //將圖變成灰階
  procedure getGrayScale(tarImg, buffImg:TImage);
  //濾圖 只留深紅深藍兩色
  procedure getGBIMG(tarImg, buffImg:TImage; thr:Double);

  //去雜訊
  procedure imgDilationOrErosion(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer; isDilation: Boolean);
  procedure imgDilation(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer);
  procedure imgErosion(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer);
  
  function getGuaKernel5by5(): TInteger2DArray;
  function getGuaPro1DKernel(kLen: Integer; BlockPixelAmt: Integer): TDouble1DArray;
  
  procedure cloneImgSetting(var clonee:TImage; template:TImage);

  procedure showProcessedImg(ImgFocused, buffImg, resultImg: TImage);

  procedure minusImg12ToImgTar(var img1, img2, imgTar: Timage; bpp: Integer);

  procedure HistogramEqualize(img: TImage; bpp: Integer);

  procedure subSample(tarMap, buffMap: TBitmap; sbNumber: Integer);

  function getIntegralMatix(tarBmp: TBitmap): TInteger2DArray;
  function getAreaFromIMatrix(x1, y1, x2, y2: Integer; imW, imH: Integer; iMatrix: TInteger2DArray): Integer;
  procedure doLocalThreshold(tarBmp, buffBmp: TBitmap; iMatrix: TInteger2DArray; kW, kH: Integer);
  procedure guassianBlurWithIM(tarBmp, buffBmp: TBitmap; xBPN, yBPN: Integer; xbN, ybN: Integer; iMatrix: TInteger2DArray);
  
