procedure convolution(tarImg, buffImg: TImage; kernel: TInteger2DArray; kW, kH: Integer; bpp: Integer; ratio: double);
var
i, j, sC, pIdx: Integer;
kCIdx: Integer;//kernel center index
kESize: Integer;//kernel edge size
tarP, buffP: PByteArray;
sum: array of double;
pixC, xIdx, yIdx: Integer; //pixel counter
rowC, colC: Integer;
begin // kernel size can be only odd number and is greater than 1
    setLength(sum, bpp);

    //implement convolution, ignore border part
    for i := kH to (tarImg.Picture.Bitmap.Height-(kH+1)) do
        for j := kW to (tarImg.Picture.Bitmap.Width-(kW+1)) do
        begin
            //èˆ‡kernelåšconvolution
            //æ¯å€‹channelåˆ†é–‹åš
                //row * column
            for sC :=0 to bpp-1 do
              sum[sC] := 0;
              
            for rowC := (0 - kH) to (0 + kH) do
            begin
                yIdx := i + rowC;
                tarP := tarImg.Picture.Bitmap.Scanline[yIdx];

                for colC := (0 - kW) to (0 + kW) do
                begin
                    xIdx := (j + colC) * bpp;
                    for sC :=0 to bpp-1 do
                      sum[sC] := sum[sC] + tarP[xIdx+sC] * kernel[rowC+kH][colC+kW];
                end;
            end;
            buffP := buffImg.Picture.Bitmap.Scanline[i];

            for sC :=0 to bpp-1 do
            begin
              sum[sC] := sum[sC] * ratio;
              
              if sum[sC] > 255 then
              begin
                sum[sC] := 255;
              end
              else if sum[sC] < 0 then
                sum[sC] := 0;
                
              buffP[(j*bpp)+sC] := Round(sum[sC]);
            end;
        end;
end;

procedure getGrayScale(tarImg, buffImg:TImage);
var
   i, j, rV, r, g, b: Integer;
   tarP, buffP: PByteArray;
begin
     for i:=0 to tarImg.Picture.Bitmap.Height-1 do
     begin
          tarP := tarImg.Picture.Bitmap.Scanline[i];
          buffP:= buffImg.Picture.Bitmap.Scanline[i];
          for j:=0 to tarImg.Picture.Bitmap.Width-1 do
          begin
               b := j * 3;
               g := b+1;
               r := g+1;
               // B G R (0.21 R + 0.72 G + 0.07 B.)
               rV := Round( tarP[b]*0.07 + tarP[g]*0.72 + tarP[r]*0.21 );
          
               if rV > 255 then
                  rV := 255;
          
               buffP[b] := rV;
               buffP[g] := rV;
               buffP[r] := rV;          
          end;
     end;
end;

procedure getGBIMG(tarImg, buffImg:TImage; thr:Double);
var
   isRed, isBlue: Boolean;
   ratio: Double;
   i, j, rV, r, g, b: Integer;
   tarP, buffP: PByteArray;
begin
     for i:=0 to tarImg.Picture.Bitmap.Height-1 do
     begin
          tarP := tarImg.Picture.Bitmap.Scanline[i];
          buffP:= buffImg.Picture.Bitmap.Scanline[i];
          for j:=0 to tarImg.Picture.Bitmap.Width-1 do
          begin
               b := j*3;
               g := b+1;
               r := g+1;
               // if r,b channel is higher than 2 times sum of the other two channels
               isRed := false;
               isBlue := false;
          
               //if it's red?
               ratio := (tarP[r]+1) / (tarP[g] + tarP[b]+1);
               if ratio >= thr then
               begin
                  isRed := True;
               end
               else
               begin
                  ratio := (tarP[b]+1) / (tarP[g] + tarP[r]+1); 

                  if ratio >= thr then
                     isBlue := True;    
               end;
          
               if isRed = True then
               begin
                    buffP[r] := 255;
                    buffP[b] := 0;
                    buffP[g] := 0;  
               end
               else if isBlue = True then
               begin
                    buffP[r] := 0;
                    buffP[b] := 255;
                    buffP[g] := 0; 
               end
               else
               begin
                    buffP[r] := 0;
                    buffP[b] := 0;
                    buffP[g] := 0; 
               end;
                       
          end;
     end;   
end;

procedure imgDilationOrErosion(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer; isDilation: Boolean);
var
i, j, sC, pIdx, kW_ofset, kH_ofset, strWPL, maxWPL: Integer;
kH_CIdx , kW_CIdx : Integer;//kernel center index
kH_ESize, kW_ESize: Integer;//kernel edge size
tarP, buffP: PByteArray;
maxv: array of Integer;
xIdx, yIdx: Integer; //pixel counter
rowC, colC: Integer;
begin // kernel size can be only odd number and is greater than 1
    kH_CIdx := Floor(KS_H-1/2);
    kW_CIdx := Floor(KS_W-1/2);
    kH_ESize:= kH_CIdx; // floor(5/2) = 2(¬Oindex¤]¬Oedge size)
    kW_ESize:= kW_CIdx;
    
    setLength(maxv, bpp);
    //initialize unprocessed pixel with black
    kW_ofset := buffImg.Picture.Bitmap.Width-(kW_ESize+1);
    kH_ofset := buffImg.Picture.Bitmap.Height-(kH_ESize+1);
    maxWPL := buffImg.Picture.Bitmap.Width * 3 - 1;
    strWPL := (tarImg.Picture.Bitmap.Width-(kW_ESize))*3;
    for i:=0 to buffImg.Picture.Bitmap.Height-1 do
    begin
         buffP := buffImg.Picture.Bitmap.Scanline[i];
         if (i < kH_CIdx) or (i > kH_ofset) then
         begin //¥þ³¡ªì©l¤Æ¬°0
              for j:=0 to maxWPL do
              begin
                   buffP[j] := 0;
              end;     
         end
         else
         begin
             for j:=0 to kW_CIdx*3-1 do
             begin
                   buffP[j] := 0;
             end; 
             for j:=strWPL to maxWPL do
             begin
                   buffP[j] := 0;
             end; 
         end;
    end;
    
    //implement convolution, ignore border part
    for i := kH_CIdx to (tarImg.Picture.Bitmap.Height-(kH_ESize+1)) do
        for j := kW_CIdx to (tarImg.Picture.Bitmap.Width-(kW_ESize+1)) do
        begin
            //»Pkernel°µconvolution
            //¨C­Óchannel¤À¶}°µ
                //row * column
            for sC :=0 to bpp-1 do
            begin
              if isDilation = True then
              begin
                 maxv[sC] := 0;
              end
              else
              begin
                 maxv[sC] := 255;
              end;
            end;
              
            for rowC := (0 - kH_ESize) to (0 + kH_ESize) do
            begin
                yIdx := i + rowC;
                tarP := tarImg.Picture.Bitmap.Scanline[yIdx];

                for colC := (0 - kW_ESize) to (0 + kW_ESize) do
                begin
                    xIdx := (j + colC) * bpp;
                    for sC :=0 to bpp-1 do
                    begin
                      if isDilation = True then
                      begin
                         maxv[sC] := Max(maxv[sC], tarP[xIdx+sC]);
                      end
                      else
                      begin
                         maxv[sC] := Min(maxv[sC], tarP[xIdx+sC]);
                      end;
                    end;
                end;
            end;
            
            buffP := buffImg.Picture.Bitmap.Scanline[i];

            for sC :=0 to bpp-1 do
            begin
              buffP[(j*bpp)+sC] := maxv[sC];
            end;
        end;
end;

procedure imgDilation(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer);
begin
     imgDilationOrErosion(tarImg, buffImg,KS_W, KS_H, bpp, True);
end;
procedure imgErosion(tarImg, buffImg: TImage; KS_W, KS_H: Integer; bpp: Integer);
begin
     imgDilationOrErosion(tarImg, buffImg,KS_W, KS_H, bpp, False);
end;

function getGuaPro1DKernel(kLen: Integer; BlockPixelAmt: Integer): TDouble1DArray;
var
   GuaPK: TDouble1DArray;
begin
     if (BlockPixelAmt > 0) and ((KLen = 3) or (KLen = 5)) then
     begin
          SetLength(GuaPK, KLen);
          if kLen = 3 then
          begin
               //­ì¾÷²v¬O 0.242 0.383 0.242 sum=0.867
               //·Q­nsum¬°1 ¦]¦¹¦¹¤T¾÷²v ¬Ò­¼¥H(1/0.867) ±o¨ì¤U¦Cµ²ªG
               GuaPK[0] := 0.279/BlockPixelAmt;
               GuaPK[1] := 0.442/BlockPixelAmt;
               GuaPK[2] := 0.279/BlockPixelAmt;     
          end
          else
          begin
               GuaPK[0] := 0.062/BlockPixelAmt;  //0.061 + 0.01
               GuaPK[1] := 0.244/BlockPixelAmt;  //0.242 + 0.02
               GuaPK[2] := 0.388/BlockPixelAmt;  //0.383 + 0.05
               GuaPK[3] := 0.244/BlockPixelAmt;  //0.242 + 0.02
               GuaPK[4] := 0.062/BlockPixelAmt;  //0.061 + 0.01
          end;     
     end
     else
     begin
         SetLength(GuaPK, 1);
         GuaPK[0] := 1/BlockPixelAmt;
     end;
     Result := GuaPK;
end;

//Acutally it's 7 by 7, but values in the border are too small and be truncated 
function getGuaKernel5by5(): TInteger2DArray;
var
   i, j, size, invP, arrP, arrDir: Integer;
   GBKernel, arr: TInteger2DArray;
begin
     size := 5;// 5 by 5 kernel
     invP := Round((size - 1)/2); // inverse point
     SetLength(arr, invP+1);
     for i:=0 to invP do
         SetLength(arr[i], size);
     {
         If size changed, here is the only place needed to be modified
     }
     arr[0][0] := 1; arr[0][1] := 4; arr[0][2] := 7; arr[0][3] := 4; arr[0][4] := 1;
     arr[1][0] := 4; arr[1][1] := 16; arr[1][2] := 26; arr[1][3] := 16; arr[1][4] := 4;
     arr[2][0] := 7; arr[2][1] := 26; arr[2][2] := 41; arr[2][3] := 26; arr[2][4] := 7;

     //Start to assign value to GBKernel, according to values in arr.         
     SetLength(GBKernel, size);
     for i:=0 to (size-1) do
         SetLength(GBKernel[i], size);

     arrP:=0;
     arrDir := 1;
     for i:=0 to size-1 do
     begin
          for j:=0 to size-1 do
          begin
               GBKernel[i][j] := arr[arrP][j];
          end;

          arrP := arrP + arrDir;
          if arrP = invP then
             arrDir := -1;
     end;
     
     result:= GBKernel;
end;

procedure cloneImgSetting(var clonee:TImage; template:TImage);
begin
     clonee := TImage.Create(Form1);
     clonee.Picture.Bitmap.PixelFormat := template.Picture.Bitmap.PixelFormat;
     clonee.Picture.Bitmap.Width := template.Picture.Bitmap.Width;
     clonee.Picture.Bitmap.Height := template.Picture.Bitmap.Height;
end;

procedure showProcessedImg(ImgFocused, buffImg, resultImg: TImage);
begin
     buffImg.Picture.Bitmap.Assign(ImgFocused.Picture.Bitmap);
     ImgFocused.Picture.Bitmap.Assign(resultImg.Picture.Bitmap);
end;

procedure minusImg12ToImgTar(var img1, img2, imgTar: Timage; bpp: Integer);
var
i, j, xI, sC: Integer;
P1, P2, tarP: PByteArray;
pixelAvg, tmp: Integer;
begin
    for i:=0 to imgTar.Picture.Bitmap.Height-1 do
    begin
        P1 := img1.Picture.Bitmap.ScanLine[i];
        P2 := img2.Picture.Bitmap.ScanLine[i];
        tarP := imgTar.Picture.Bitmap.ScanLine[i];

        for j:= 0 to (imgTar.Picture.Bitmap.Width-1) do
        begin
            xI := j * bpp;
            for sC:= xI to (xI+bpp-1) do
            begin
              tmp := P1[sC] - P2[sC];
              
              if tmp > 255 then
              begin
                tmp := 255;
              end
              else if tmp < 0 then
                tmp := 0;

              tarP[sC] := tmp;

            end;

        end;
    end;
end;

procedure HistogramEqualize(img: TImage; bpp: Integer);
var
i, j, sC, idx:Integer;
P: PByteArray;
b, g, r: Integer;
pixel, count : array[0..2] of Integer;
ft: double;
cArea: Integer;
LUT : array[0..2] of array[0..255] of byte;
Histogram : array[0..2] of array[0..255] of Integer;
begin
    b := 0;
    g := 1;
    r := 2;
    //¥Ø«e¥»µ{¦¡¥u°µ²Ä¤@­Ópixel
    for i:=0 to 255 do
    begin
        Histogram[b][ i ] := 0;
        Histogram[g][ i ] := 0;
        Histogram[r][ i ] := 0;
    end;
    
    count[0]:= 0;
    count[1]:= 0;
    count[2]:= 0;

    for i:=0 to img.Picture.Bitmap.Height-1 do
    begin
        P := img.Picture.Bitmap.ScanLine[i];

        for j:=0 to img.Picture.Bitmap.Width-1 do
        begin
             for sC:=0 to 2 do
             begin
                pixel[sC]:= P[j * bpp + sC];

                if pixel[sC] > 0 then
                begin
                  Histogram[sC][ pixel[sC] ] := Histogram[sC][ pixel[sC] ] + 1;
                  count[sC] := count[sC] + 1;
                end;
             end;
        end;
    end;

    //¨ú±oLookup table
    
    //make lookup table
    for sC:=0 to 2 do
    begin
        ft := 255 / (count[sC]);
        cArea := 0;
        for i:=0 to 255 do
        begin
            cArea:= cArea + Histogram[sC][i];
            LUT[sC][i] := round( cArea * ft );
        end;
    end;
    // Cover original value with lookup table
    for i:=0 to img.Picture.Bitmap.Height-1 do
    begin
      P := img.Picture.Bitmap.ScanLine[i];

      for j:=0 to img.Picture.Bitmap.Width-1 do
      begin
          idx := j * 3;
          for sC :=0 to 2 do
          begin
             P[idx + sC] := LUT[sC][ P[idx + sC] ];
          end;
      end;
    end;
    img.refresh;
end;

procedure subSample(tarMap, buffMap: TBitmap; sbNumber: Integer);
var
   tarSCLine, buffSCLine: PByteArray;
   tXI, bXI, tX, tY, bX, bY, j, tW, tH, bW, bH, txStep, tyStep: Integer;
begin
  if (sbNumber > 0) and ( (tarMap.Width > 0) and (buffMap.Height > 0) ) then
  begin //½T©w¶i¨Óªº­È¦Xªk  
       tW := tarMap.Width;
       tH := tarMap.Height;
       bW := buffMap.Width;
       bH := buffMap.Height;

       txStep := sbNumber;//3­Ópixel
       tyStep := sbNumber;
       
       tY := 0;
       bY := 0;
       while (tY < tH) and (bY < bH) do
       begin
            tarSCLine := tarMap.Scanline[tY];
            buffSCLine := buffMap.Scanline[bY];
            tX := 0;
            bX := 0;
            while (tX < tW) and (bX < bW) do
            begin
                 tXI := tX * 3;
                 bXI := bX * 3;
            
                 buffSCLine[bXI] := tarSCLine[tXI];
                 buffSCLine[bXI+1] := tarSCLine[tXI+1];
                 buffSCLine[bXI+2] := tarSCLine[tXI+2];

                 tX := tX + txStep;   
                 bX := bX + 1;  
            end;

            tY := tY + tyStep; 
            bY := bY + 1; 
       end;
  end;      
end;

function getAreaFromIMatrix(x1, y1, x2, y2: Integer; imW, imH: Integer; iMatrix: TInteger2DArray): Integer;
begin// if invalid return -1
    if ((x1 >= 0) and (y1 >= 0)) and ((x2 < imW) and (y2 < imH)) then
    begin
        if (x1 > 0) and (y1 > 0) then
        begin//¥¿±`±¡ªp
             Result := iMatrix[y2][x2] - iMatrix[y1-1][x2] - iMatrix[y2][x1-1] + iMatrix[y1-1][x1-1];
        end
        else if (x1 > 0) then
        begin//³Ì¤W¤èªºrow
             Result := iMatrix[y2][x2] - iMatrix[y2][x1-1];
        end
        else if (y1 > 0) then
        begin//³Ì¥ª¤èªºcolumn
             Result := iMatrix[y2][x2] - iMatrix[y1-1][x2];
        end
        else//³Ì¥ª¤W¨¤
        begin
             Result := iMatrix[y2][x2];
        end;
    end
    else
    begin//invalid
         Result := -1;
    end;
end;

function getIntegralMatix(tarBmp: TBitmap): TInteger2DArray;
var
   xI, nxpI, yI, j, channel: Integer;
   tSCLine: PByteArray;
   iMatrix: TInteger2DArray;   
begin
  if (tarBmp.Width > 0) and (tarBmp.Height > 0) then
  begin
       channel := 0;

       SetLength(iMatrix, tarBmp.Height);
       for yI:=0 to tarBmp.Height-1 do
       begin
            SetLength(iMatrix[yI], tarBmp.Width);
            tSCLine := tarBmp.ScanLine[yI];
            for xI:= 0 to tarBmp.Width-1 do
            begin
                 nxpI := xI * 3 + channel;
          
                 if (xI >= 1) and (yI >= 1)then
                 begin//¥¿±`³B²z
                      iMatrix[yI][xI] := tSCLine[nxpI] + iMatrix[yI-1][xI] + iMatrix[yI][xI-1] - iMatrix[yI-1][xI-1];
                 end
                 else if (xI >= 1) then//³Ì¤W¤èªºrow
                 begin
                      iMatrix[yI][xI] := tSCLine[nxpI] + iMatrix[yI][xI-1];
                 end
                 else if (yI >= 1) then//³Ì¥ª¤èªºColumn
                 begin
                      iMatrix[yI][xI] := tSCLine[nxpI] + iMatrix[yI-1][xI];
                 end
                 else//(0,0)
                 begin
                      iMatrix[yI][xI] := tSCLine[nxpI];
                 end;     
            end;
       end;
  end;
  Result := iMatrix;
end;

procedure doLocalThreshold(tarBmp, buffBmp: TBitmap; iMatrix: TInteger2DArray; kW, kH: Integer);
var
   tSCLine, bufSCLine: PByteArray;
   xI, xpI, yI, pArea, mean, totalPixel, topY, leftX, btmY, rightX: Integer;
begin
     totalPixel := (kW*2+1) * (kH*2+1);

     for yI:=0 to tarBmp.Height-1 do
     begin
          topY := yI - kH - 1;
          btmY := yI + kH;
          tSCLine := tarBmp.ScanLine[yI];
          bufSCLine := buffBmp.ScanLine[yI];
          for xI:= 0 to tarBmp.Width-1 do
          begin
               leftX:= xI - kW - 1;
               rightX:= xI + kW;
               xpI := xI * 3;
               //¤£¯à¶W¹L½d³ò
               if ((leftX >= 0) and (topY >= 0)) and ((rightX < tarBmp.Width) and (btmY < tarBmp.Height))then
               begin
                   pArea := iMatrix[btmY][rightX] - iMatrix[topY][rightX] - iMatrix[btmY][leftX] + iMatrix[topY][leftX];
                   mean := Round(pArea / totalPixel);
                   mean := mean - 5;
                   if mean > 255 then
                      mean := 255;
                   if mean < 0 then
                      mean := 0;
                   if tSCLine[xpI] < mean then 
                   begin
                        bufSCLine[xpI]   := 255;
                        bufSCLine[xpI+1] := 255;
                        bufSCLine[xpI+2] := 255; 
                        {bufSCLine[xpI]   := 255-tSCLine[xpI];
                        bufSCLine[xpI+1] := 255-tSCLine[xpI+1];
                        bufSCLine[xpI+2] := 255-tSCLine[xpI+2];}  
                   end
                   else
                   begin
                        bufSCLine[xpI]   := 0;
                        bufSCLine[xpI+1] := 0;
                        bufSCLine[xpI+2] := 0;     
                   end;
               end
               else//ÄÝ©óÃä¬É°Ï°ì, ª½±µ³]0
               begin
                    bufSCLine[xpI] := 0;
                    bufSCLine[xpI+1] := 0;
                    bufSCLine[xpI+2] := 0;     
               end;
          end;
     end;
end;

procedure guassianBlurWithIM(tarBmp, buffBmp: TBitmap; xBPN, yBPN: Integer; xbN, ybN: Integer; iMatrix: TInteger2DArray);
// xyBPN:xy axis pixels amount in a block; xbN: x axis blocks amount; ybN: y axis blocks amount;
var
   imW, imH: Integer;
   XBToCLen, YBToCLen, XSideBlockPixelAmt, YSideBlockPixelAmt, XSidePixelAmt, YSidePixelAmt: Integer;// length(how many pixels) from Border to Center(the point to be processed)
   xI, yI, pIdx, xSingBN, ySingBN: Integer;
   x1, y1, x2, y2, nXIidx, nYIidx, xC, yC: Integer;
   blockArea, value, testValue: Integer;
   sum : double;
   GuaXDP, GuaYDP: TDouble1DArray;
   tarSCLine, buffSCLine: PByteArray;
begin
     GuaXDP := getGuaPro1DKernel(xbN, xBPN); 
     xSingBN := Round((xbN - 1) / 2);// X³æÃäªºblockÁ`¼Æ((5-1)/2 = 2)
     GuaYDP := getGuaPro1DKernel(ybN, yBPN); 
     ySingBN := Round((ybN - 1) / 2);// Y³æÃäªºblockÁ`¼Æ((5-1)/2 = 2)

     //calculate border length
     //¤¤¤ßÂIpixel ®Ç³æÃä(¥ª©Î¥k ¤W©Î¤U)©Ò¦³ªºblock¼Æ * block¸Ì­±ªºpixel¼Æ + ¤¤¶¡block¦Û¤v¸Ì­±¥bÃäªºpixel¼Æ
     XSidePixelAmt := Round((xBPN-1) / 2);
     YSidePixelAmt := Round((yBPN-1) / 2);

     XBToCLen := xSingBN * xBPN + XSidePixelAmt;
     YBToCLen := ySingBN * yBPN + YSidePixelAmt;

     XSideBlockPixelAmt := xSingBN * xBPN;
     YSideBlockPixelAmt := ySingBN * yBPN;
     
     //get img width and height
     imW := buffBmp.Width;
     imH := buffBmp.Height;
     //
     //­n«OÃÒ xbN  ybN ¦Ü¤Ö¤@­Ó¬O3 ©Î 5 (¨â­Ó³£­n>=1)
     //       xBPN yBPN ­n¬O ©_¼Æ       (¨â­Ó³£­n>=1)
     //§_«h°µ¤F·|¦³°ÝÃD
     if ((xbN < 1) or (ybN < 1)) or (((xbN mod 2)= 0) or ((ybN mod 2)= 0)) then
     begin
          GuaXDP[0] := 1;
          GuaYDP[0] := 1;
     end;

       
     if (GuaXDP[0] < 0.99) or (GuaYDP[0] < 0.99) then
     begin//¦Ü¤Ö¨ä¤¤¤@­Ó¤£µ¥©ó1¤~­n°µ
         for yI:= YBToCLen to imH-(YBToCLen+1) do
         begin
              tarSCLine := tarBmp.ScanLine[yI];
              buffSCLine := buffBmp.ScanLine[yI];
              for xI := XBToCLen to imW-(XBToCLen+1) do
              begin
                   //do guassian blur
                   sum := 0;
                   
                   nXIidx := xI - XSideBlockPixelAmt;// ­pºâ ³Ì¥ªÃäblockªº¤¤¤ßÂI¦ì¸m
                   xC := 0;
                   while xC < xbN do
                   begin
                       nYIidx := yI - YSideBlockPixelAmt;// ­pºâ ³Ì¤W­±blockªº¤¤¤ßÂI¦ì¸m 
                       yC := 0;
                       while yC < ybN do
                       begin
                           //±q¨ú±o³æ1 blockªºArea * Xkernel * Ykernel = ©Ò¯¸ªº¥÷¶q ¨Ã²Ö¥[
                           x1 := nXIidx - XSidePixelAmt;
                           y1 := nYIidx - YSidePixelAmt;
                           x2 := nXIidx + XSidePixelAmt;
                           y2 := nYIidx + YSidePixelAmt;
                           blockArea := getAreaFromIMatrix(x1, y1, x2, y2, imW, imH, iMatrix);

                           sum := sum + (blockArea * GuaXDP[xC] * GuaYDP[yC]);
                           
                           nYIidx := nYIidx + yBPN;
                           yC := yC + 1;
                       end;
                       nXIidx := nXIidx + xBPN;
                       xC := xC + 1;
                   end;

                   //¨ú±o¸ÓÂI guassian blur ªºµ²ªG
                   value := Round(sum);
                   pIdx := xI * 3;
                   if value > 255 then
                   begin
                        value := 255;  
                   end;
                   testValue := tarSCLine[pIdx+1];
                   buffSCLine[pIdx]   := value;
                   buffSCLine[pIdx+1] := value;
                   buffSCLine[pIdx+2] := value;
              end;
         end;
     end;                        
end;
