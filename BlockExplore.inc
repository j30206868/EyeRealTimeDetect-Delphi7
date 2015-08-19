constructor TBlockExplore.Create(In_Img :TImage; In_BlockVMinW, In_BlockVMinH:Integer);
begin
     TopBund := -1;
     BtmBund := -1;
     LeftBund := -1;
     RightBund := -1;
     Img := In_Img;

     BlockVMinW := In_BlockVMinW;
     BlockVMinH := In_BlockVMinH;

     SrhChannelMinV := 50;

     DBG_DrawOnDBGC := False;
     DBG_Channel   := 0;//green
end;

function TBlockExplore.isValidBlock():Boolean;
var
   w, h: Integer;
begin
     result := True;
     if (TopBund = -1) or (BtmBund = -1) then
        result := False;
     if (RightBund = -1) or (LeftBund = -1) then
        result := False;
     w := RightBund - LeftBund;
     h := BtmBund - TopBund;
     if (w <= BlockVMinW) and (h <= BlockVMinH) then
     begin
          result := False;
     end;
end;

procedure TBlockExplore.drawBlockOnImg(tarImg: TBitmap; subRate: Integer);
var
  oTop, oBtm, oRight, oLeft, x1, y1, x2, y2: Integer;
  i, j, idx, count:Integer;
  pSCLine : PByteArray;
begin
     //記錄原本的
     oTop := TopBund;
     oBtm := BtmBund;
     oRight := RightBund;
     oLeft := LeftBund;

     //因Img跟要畫的tarImg大小可能是不一樣的 所以要先做轉換
     TopBund := TopBund * subRate;
     BtmBund := BtmBund * subRate;
     RightBund := RightBund * subRate;
     LeftBund := LeftBund * subRate;

     //防止 Top Btm Left Right其中任何一個Block的值超出傳入的tarImg的邊界
     setFourBundToValid(tarImg); 

     //用canvas畫 會顛倒 要轉換座標 
     x1 := LeftBund;
     y1 := TopBund;
     x2 := RightBund;
     y2 := BtmBund;    
     
     tarImg.Canvas.Pen.Width := subRate;
     tarImg.Canvas.Pen.Color := $00ff00;
     tarImg.Canvas.MoveTo(x1, y1);
     tarImg.Canvas.LineTo(x2, y1);
     tarImg.Canvas.LineTo(x2, y2);
     tarImg.Canvas.LineTo(x1, y2);
     tarImg.Canvas.LineTo(x1, y1);
     //用canvas畫 會顛倒 要轉換座標
     
     {for i:=TopBund to BtmBund do
     begin
          pSCLine := tarImg.Scanline[i]; 
          j:=LeftBund;
          count := 0;
          while (j>=LeftBund) and (j<=RightBund) do
          begin
               idx := j * 3;
               pSCLine[idx]   := 0;  //b
               pSCLine[idx+1] := 255;//g
               pSCLine[idx+2] := 0;  //r 

               if (i=TopBund) or (i=BtmBund) then
               begin
                    j:=j+1;
               end
               else if count < 2 then
               begin
                    j := RightBund;
               end
               else
                   break;
               count := count+1;     
          end;    
     end;}
     //tarImg.Refresh;

     //還原
     TopBund := oTop;
     BtmBund := oBtm;
     RightBund := oRight;
     LeftBund := oLeft;
end;

procedure TBlockExplore.ExploreFromXY(channel, oX, oY, itrCount, itrLimit: Integer);
begin
  //防止不合法的(oX, oY被傳入)
  if oX < 0 then
     oX := 0;
  if oY < 0 then
     oY := 0;
  if oX >= Img.Picture.Bitmap.Width then
     oX := Img.Picture.Bitmap.Width-1;
  if oY >= Img.Picture.Bitmap.Height then
     oY := Img.Picture.Bitmap.Height-1;

  //if ((oX >= 0) and (oX < Img.Picture.Bitmap.Width)) and
  //   ((oY >= 0) and (oY < Img.Picture.Bitmap.Height)) then
  //begin
      //以(oX, oY)為中心點 以 米 字型向外找尋框框的邊界
      HorizontalSearch(channel, oX, oY, itrCount, itrLimit); //從水平線延伸找尋框框的邊界
      VerticalSearch(channel, oX, oY, itrCount, itrLimit);   //從垂直線延伸找尋框框的邊界
      LTRDiagnal(channel, oX, oY, itrCount, itrLimit);       //從對角線＼延伸找尋框框的邊界
      RTLDiagnal(channel, oX, oY, itrCount, itrLimit);       //從對角線／延伸找尋框框的邊界
  //end;  
end;

procedure TBlockExplore.HorizontalSearch(channel, oX, oY, itrCount, itrLimit:Integer);
var
  i, tmpR, tmpL, v: Integer;
  pSCLine : PByteArray;
begin
     if oY < Img.Picture.Bitmap.Height then
     begin
       pSCLine := Img.Picture.Bitmap.ScanLine[oY];

       //往右
       tmpR := oX;
       for i:=oX+1 to Img.Picture.Bitmap.Width-1 do
       begin
            v := pSCLine[ i * 3 + channel ];
            if v > SrhChannelMinV then
            begin
               tmpR := i;

               //debug
               searchDebugging(pSCLine, i); 
            end
            else
                break;     
       end;
       if (RightBund = -1) or (tmpR > RightBund) then
          RightBund := tmpR;
       drawDebugLine();
       //往左
       tmpL := oX;
       for i:=oX-1 downto 0 do
       begin
            v := pSCLine[ i * 3 + channel ];
            if v > SrhChannelMinV then
            begin
               tmpL := i;

               //debug
               searchDebugging(pSCLine, i);
            end
            else
                break;     
       end;
       if (LeftBund = -1) or (tmpL < LeftBund) then
          LeftBund := tmpL;
       drawDebugLine();
       //檢查是否需要反彈
       if (itrCount < itrLimit) and (tmpR <> oX) then
          ExploreFromXY(channel, tmpR, oY, itrCount+1, itrLimit);
       if (itrCount < itrLimit) and (tmpL <> oX) then
          ExploreFromXY(channel, tmpL, oY, itrCount+1, itrLimit);
     end;
end;

procedure TBlockExplore.VerticalSearch(channel, oX, oY, itrCount, itrLimit:Integer);
var
  i, xIdx, tmpT, tmpB, v: Integer;
  pSCLine : PByteArray;
begin
    if oX < Img.Picture.Bitmap.Width then
    begin
      xIdx := oX * 3 + channel;
    
      //往下
      tmpB := oY;
      for i:=oY+1 to Img.Picture.Bitmap.Height-1 do
      begin
          pSCLine := Img.Picture.Bitmap.ScanLine[i];
          v := pSCLine[ xIdx ];
          if v > SrhChannelMinV then
          begin
             tmpB := i;

             //debug
             searchDebugging(pSCLine, oX);
          end
          else
              break;     
      end;
      if (BtmBund = -1) or (tmpB > BtmBund) then
          BtmBund := tmpB;
      drawDebugLine();
      //往上
      tmpT := oY;
      for i:=oY-1 downto 0 do
      begin
          pSCLine := Img.Picture.Bitmap.ScanLine[i];
          v := pSCLine[ xIdx ];
          if v > SrhChannelMinV then
          begin
             tmpT := i;

             //debug
             searchDebugging(pSCLine, oX);
          end
          else
              break;     
      end;
      if (TopBund = -1) or (tmpT < TopBund) then
          TopBund := tmpT;
      drawDebugLine();
      //檢查是否需要反彈
      if (itrCount < itrLimit) and (tmpB <> oY) then
          ExploreFromXY(channel, oX, tmpB, itrCount+1, itrLimit);
      if (itrCount < itrLimit) and (tmpT <> oY) then
          ExploreFromXY(channel, oX, tmpT, itrCount+1, itrLimit);
    end;
end;

procedure TBlockExplore.LTRDiagnal(channel, oX, oY, itrCount, itrLimit:Integer);
var
  xI, yI, tmpT, tmpB, tmpL, tmpR, v: Integer;
  pSCLine : PByteArray;
begin
     //往右下
     tmpB := oY;
     tmpR := oX;
     xI := oX;
     yI := oY;
     while (xI < Img.Picture.Bitmap.Width) and (yI < Img.Picture.Bitmap.Height) do
     begin
          pSCLine := Img.Picture.Bitmap.ScanLine[yI];
          v := pSCLine[ xI * 3 + channel ]; 
                  
          if v > SrhChannelMinV then
          begin
             tmpR := xI;
             tmpB := yI;

             //debug
             searchDebugging(pSCLine, xI);
          end
          else
              break;
                  
          xI := xI+1;
          yI := yI+1;
     end;
     //更新right Btm的值
     if (RightBund = -1) or (tmpR > RightBund) then
        RightBund := tmpR;
     if (BtmBund = -1) or (tmpB > BtmBund) then
        BtmBund := tmpB;
     drawDebugLine();
     //往左上
     tmpT := oY;
     tmpL := oX;
     xI := oX;
     yI := oY;
     while (xI >= 0) and (yI >= 0) do
     begin
          pSCLine := Img.Picture.Bitmap.ScanLine[yI];
          v := pSCLine[ xI * 3 + channel ]; 
                  
          if v > SrhChannelMinV then
          begin
             tmpL := xI;
             tmpT := yI;

             //debug
             searchDebugging(pSCLine, xI);
          end
          else
              break;
                  
          xI := xI-1;
          yI := yI-1;
     end;
     //更新Left Top的值
     if (LeftBund = -1) or (tmpL < LeftBund) then
        LeftBund := tmpL;
     if (TopBund = -1) or (tmpT < TopBund) then
        TopBund := tmpT;
     drawDebugLine();
     //檢查是否需要反彈
     if itrCount < itrLimit then
     begin
          if (tmpB <> oY) or (tmpR <> oX) then
             ExploreFromXY(channel, tmpR, tmpB, itrCount+1, itrLimit);
          if (tmpT <> oY) or (tmpL <> oX) then
             ExploreFromXY(channel, tmpL, tmpT, itrCount+1, itrLimit); 
     end;
end;

procedure TBlockExplore.RTLDiagnal(channel, oX, oY, itrCount, itrLimit:Integer);
var
  xI, yI, tmpT, tmpB, tmpL, tmpR, v: Integer;
  pSCLine : PByteArray;
begin
     //往右上
     tmpT := oY;
     tmpR := oX;
     xI := oX;
     yI := oY;
     while (xI < Img.Picture.Bitmap.Width) and (yI >= 0) do
     begin
          pSCLine := Img.Picture.Bitmap.ScanLine[yI];
          v := pSCLine[ xI * 3 + channel ]; 
                  
          if v > SrhChannelMinV then
          begin
             tmpR := xI;
             tmpT := yI;

             //debug
             searchDebugging(pSCLine, xI);
          end
          else
              break;
                  
          xI := xI+1;
          yI := yI-1;
     end;
     if (RightBund = -1) or (tmpR > RightBund) then
        RightBund := tmpR;
     if (TopBund = -1) or (tmpT < TopBund) then
        TopBund := tmpT;
     drawDebugLine();
     //往左下
     tmpB := oY;
     tmpL := oX;
     xI := oX;
     yI := oY;
     while (xI >= 0) and (yI < Img.Picture.Bitmap.Height) do
     begin
          pSCLine := Img.Picture.Bitmap.ScanLine[yI];
          v := pSCLine[ xI * 3 + channel ]; 
                  
          if v > SrhChannelMinV then
          begin
             tmpL := xI;
             tmpB := yI;

             //debug
             searchDebugging(pSCLine, xI);
          end
          else
              break;
                  
          xI := xI-1;
          yI := yI+1;
     end;
     if (LeftBund = -1) or (tmpL < LeftBund) then
        LeftBund := tmpL;
     if (BtmBund = -1) or (tmpB > BtmBund) then
        BtmBund := tmpB;
     drawDebugLine();
     //檢查是否需要反彈    
     if itrCount < itrLimit then
     begin
          if (tmpT <> oY) or (tmpR <> oX) then
             ExploreFromXY(channel, tmpR, tmpT, itrCount+1, itrLimit);
          if (tmpB <> oY) or (tmpL <> oX) then
             ExploreFromXY(channel, tmpL, tmpB, itrCount+1, itrLimit);
     end;
end;

procedure TBlockExplore.setFourBundToValid(tarImg: TBitmap);
begin
     if TopBund < 0 then
        TopBund := 0;
     if TopBund >= tarImg.Height then
        TopBund := tarImg.Height-1;
        
     if BtmBund < 0 then
        BtmBund := 0;
     if BtmBund >= tarImg.Height then
        BtmBund := tarImg.Height-1;

     if LeftBund < 0 then
        LeftBund := 0;
     if LeftBund >= tarImg.Width then
        LeftBund := tarImg.Width-1;

     if RightBund < 0 then
        RightBund := 0;
     if RightBund >= tarImg.Width then
        RightBund := tarImg.Width-1;
end;  

procedure TBlockExplore.changeImg(New_Img: TImage);
begin
     Img := New_Img;
end;

procedure TBlockExplore.clearBundValue();
begin
     TopBund := -1;
     BtmBund := -1;
     LeftBund := -1;
     RightBund := -1;
end;
function TBlockExplore.getWidth(): Integer;
begin
     result := RightBund - LeftBund;
end;
function TBlockExplore.getHeight(): Integer;
begin
     result := BtmBund - TopBund;
end;
function TBlockExplore.getArea(): Integer;
begin
     Result := getWidth() * getHeight();
end;

procedure TBlockExplore.searchDebugging(pSCLine: PByteArray; xIdx:Integer);
begin
     if DBG_DrawOnDBGC = True then
     begin
        pSCLine[ xIdx * 3 + DBG_Channel ] := 255;
     end;
end;
procedure TBlockExplore.drawDebugLine();
begin
     if DBG_DrawOnDBGC = True then
     begin
          Img.refresh;  
          Sleep(100);
     end;
end;
