unit EyeDetect;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, LPControl, SLControlCollection, VLCommonDisplay, VLImageDisplay,
  VLDSCapture, LPComponent, SLCommonFilter, VLCommonFilter,
  VLBasicGenericFilter, VLGenericFilter, ExtCtrls, JPEG, math, StdCtrls, EyePtrnExplorer;

type
  TInteger2DArray = array of array of Integer;
  TDouble1DArray = array of Double;
  
  TForm1 = class(TForm)
    VLGenericFilter1: TVLGenericFilter;
    VLDSCapture1: TVLDSCapture;
    VLImageDisplay1: TVLImageDisplay;
    DebugList: TListBox;
    procedure VLGenericFilter1ProcessData(Sender: TObject;
      InBuffer: IVLImageBuffer; var OutBuffer: IVLImageBuffer;
      var SendOutputData: Boolean);
    procedure VLGenericFilter1Start(Sender: TObject; var AWidth,
      AHeight: Integer; AFrameDelay: Real);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

  {$i PreProcessFuncPrototype.inc}

var
  Form1: TForm1;
  curBmp, buffBmp: TBitmap;
  isEyeExprInit: Boolean;
  isBufImgsInit: Boolean;
  ImgFocused: TImage;
  GScaleBuffImg, BlurBuffImg, thSquareBuffImg: TImage;

  EyeExpr: TEyePExplorer;

  numCount : Integer;
  
implementation

{$R *.dfm}

{$i PreProcessFunc.inc}

procedure TForm1.VLGenericFilter1ProcessData(Sender: TObject;
  InBuffer: IVLImageBuffer; var OutBuffer: IVLImageBuffer;
  var SendOutputData: Boolean);
var
  subRate: Integer;
  finalBuff: TBitmap;
  iMatrix: TInteger2DArray;
  EroImg: TImage;
  DilImg: TImage;
  oldR, oldL, oldT, oldB: Integer;
begin
     subRate := 4;
     InBuffer.ToBitmap(curBmp);
     //buffBmp.PixelFormat := curBmp.PixelFormat;
     if (curBmp.Width > 0) and (curBmp.Height > 0) then
     begin
         
         ImgFocused.Picture.Bitmap.Width := Round(curBmp.Width / subRate);
         ImgFocused.Picture.Bitmap.Height := Round(curBmp.Height / subRate);
         subSample(curBmp, ImgFocused.Picture.Bitmap, subRate);

         if isBufImgsInit = False then
         begin
              cloneImgSetting(GScaleBuffImg, ImgFocused);
              cloneImgSetting(BlurBuffImg, ImgFocused);
              cloneImgSetting(thSquareBuffImg, ImgFocused);
              isBufImgsInit := True;
         end;

         getGrayScale(ImgFocused, GScaleBuffImg);
         iMatrix := getIntegralMatix(GScaleBuffImg.Picture.Bitmap);
         guassianBlurWithIM(GScaleBuffImg.Picture.Bitmap, BlurBuffImg.Picture.Bitmap, 3, 3, 3, 3, iMatrix);
         doLocalThreshold(BlurBuffImg.Picture.Bitmap, thSquareBuffImg.Picture.Bitmap, iMatrix, 5, 11);

         if isEyeExprInit = False then
         begin
              EyeExpr := TEyePExplorer.Create(thSquareBuffImg, thSquareBuffImg.Picture.Bitmap);
              isEyeExprInit := True;
         end;

         EyeExpr.searchEyePattern(DebugList);
         {EyeExpr.BlueBlockExpr.TopBund  := 6;
         EyeExpr.BlueBlockExpr.LeftBund := 13;
         EyeExpr.BlueBlockExpr.BtmBund  := 95;
         EyeExpr.BlueBlockExpr.RightBund := 123; }
         numCount := numCount + 1;
         //DebugList.Items.Add( '藍框'+inttostr(numCount)+' ('+inttostr(EyeExpr.BlueBlockExpr.LeftBund) + ',' + inttostr(EyeExpr.BlueBlockExpr.TopBund) + ') ~ (' + inttostr(EyeExpr.BlueBlockExpr.RightBund) + ',' + inttostr(EyeExpr.BlueBlockExpr.BtmBund) + ')');

         //if numCount >= 2 then
         //   DebugList.Items.Exchange(0, numCount-1);

         //EyeExpr.BlueBlockExpr.drawBlockOnImg(thSquareBuffImg.Picture.Bitmap, 1);
         //EyeExpr.RedBlockExpr.drawBlockOnImg(thSquareBuffImg.Picture.Bitmap, 1);

         Form1.Caption := '圖片大小: '+IntToStr(thSquareBuffImg.Picture.Bitmap.Width)+','+IntToStr(thSquareBuffImg.Picture.Bitmap.Height);

         OutBuffer.FromBitmap(thSquareBuffImg.Picture.Bitmap);
    end;

end;

procedure TForm1.VLGenericFilter1Start(Sender: TObject; var AWidth,
  AHeight: Integer; AFrameDelay: Real);
begin
    curBmp := TBitmap.create;
    curBmp.PixelFormat := pf24bit;
    buffBmp := TBitmap.create;
    buffBmp.PixelFormat := pf24bit;
    ImgFocused := TImage.Create(Form1);
    ImgFocused.Picture.Bitmap.PixelFormat := pf24bit;
    isEyeExprInit := False;
    isBufImgsInit := False;
    numCount :=0 ;
end;

end.
