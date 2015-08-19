unit EyePtrnExplorer;

interface

uses
  SysUtils, ExtCtrls, math, StdCtrls, Dialogs, Graphics;

type
    {$i BlockExplore_H.inc}

    TEyePExplorer = Class(TObject)  
        public
        tarImg: TImage;
        drawTarImg: TBitmap;
        BlueBlockExpr, RedBlockExpr: TBlockExplore;
        AreaRatioThr, WHRatioThr: Double;
        SrhXStep, SrhYStep, BlueSrhItrLimit, RedSrhItrLimit, BlockVMinW, BlockVMinH, eyeInterval: Integer;
        
        constructor Create(IN_Img: TImage); overload;
        constructor Create(IN_Img: TImage; IN_DrawTarImg: TBitmap); overload;
        //找Pattern
        procedure searchEyePattern(DebugList:TListBox);
        //以藍框的位置 推測紅框(使用前必須確認BlueBlockExpr已經被初始化了)
        function detectRedBlockByBlue(DebugList:TListBox): Boolean;
        //BlockExprHandler := TBlockExplore.Create(tarImg) 使用前要先初始化handler
        function xAxisSearch(channel, sX, sY, srhLenLimit: Integer; BlockExprHandler: TBlockExplore; DebugList:TListBox): Boolean;
        function isRBAreaValid(): Boolean;
    end;

implementation

{$i BlockExplore.inc}

constructor TEyePExplorer.Create(IN_Img: TImage; IN_DrawTarImg: TBitmap);
//建構元: 設定所有的threshold跟變數
begin
     AreaRatioThr := 1.6; //藍框跟紅框 面積比(大/小) 最多差多少倍 才算合法
     WHRatioThr := 1.7;   //藍框跟紅框 長寬比(大/小) 最多差多少倍 才算合法
     //框框最小的長寬限制(跟SrhXYStep有關)   
     BlockVMinW := 10;
     BlockVMinH := 5;
     //搜尋藍點時 一次跳的距離(濾波)
     SrhXStep   := 10;
     SrhYStep   := 5;
     BlueSrhItrLimit := 1;
     RedSrhItrLimit := 1; //找框框時, 反射的最大次數 

     //兩眼框框的距離
     eyeInterval := 10;

     tarImg := IN_Img;            //搜尋使用的影像(通常是把原圖subsample後的影像)
     drawTarImg := IN_DrawTarImg; //畫框框 時的 目標影像(通常是原圖)
     RedBlockExpr  := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//找紅框框 使用的框框搜尋器
     BlueBlockExpr := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//找藍框框 使用的框框搜尋器
end;

constructor TEyePExplorer.Create(IN_Img: TImage);
//建構元: 設定所有的threshold跟變數
begin
     AreaRatioThr := 1.2; //藍框跟紅框 面積比(大/小) 最多差多少倍 才算合法
     WHRatioThr := 1.5;   //藍框跟紅框 長寬比(大/小) 最多差多少倍 才算合法
     //框框最小的長寬限制(跟SrhXYStep有關)   
     BlockVMinW := 20;
     BlockVMinH := 10; 
     //搜尋藍點時 一次跳的距離(濾波) 
     SrhXStep   := 20;
     SrhYStep   := 10;
     RedSrhItrLimit := 1; //找框框時, 反射的最大次數
     BlueSrhItrLimit := 1;

     tarImg := IN_Img;            //搜尋使用的影像(通常是把原圖subsample後的影像)
     RedBlockExpr  := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//找紅框框 使用的框框搜尋器
     BlueBlockExpr := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//找藍框框 使用的框框搜尋器
end;

procedure TEyePExplorer.searchEyePattern(DebugList:TListBox);
{***********************************************************//
       搜尋TarImg(以每個Step找一次的方式), 掃過整張圖, 
            直到找出第一個符合紅藍眼鏡的pattern 
************************************************************//}
var
  hasRedBlock: Boolean;
  i, j, channel, xStep, yStep, xI: Integer;
  secondSX, secondSY: Integer;
  tarP: PByteArray;
  isFouned : Boolean;
begin
  if (drawTarImg.Height > 0) and (drawTarImg.Height > 0) then
  begin

     //設定 搜尋時 一次跳幾格(濾波, 避免更多不必要的搜尋) 
     xStep := SrhXStep;
     yStep := SrhYStep;

     isFouned := False;
   
     channel := 0;//藍的channel是0
     i := 5;
     //找尋tarImg中的藍點
     while i < tarImg.Picture.Bitmap.Height-6 do
     begin
          tarP := tarImg.Picture.Bitmap.Scanline[i];

          j:=10;
          while j < tarImg.Picture.Bitmap.Width-11 do
          begin
               xI := j * 3 + channel;
               //檢查是否為藍點
               if tarP[xI] > 0 then
               begin//找到藍點了
                   BlueBlockExpr.clearBundValue();
                   RedBlockExpr.clearBundValue();
                   BlueBlockExpr.ExploreFromXY(channel, j, i, 0 ,BlueSrhItrLimit);//從藍點 找出藍框框
    
                   //check if the block is valid(看看block的長寬 有沒有可能是眼鏡的藍框 是否太小可以直接忽略?)
                   if BlueBlockExpr.isValidBlock() <> True then
                   begin//是可能的藍框
                      BlueBlockExpr.clearBundValue();
                      isFouned := False;
                   end;

                   //右邊框框較可能找到的地方
                   //secondSX := BlueBlockExpr.RightBund + (BlueBlockExpr.RightBund - BlueBlockExpr.LeftBund);
                   //secondSY := Round( (BlueBlockExpr.TopBund + BlueBlockExpr.BtmBund)/2.0 );
                   //RedBlockExpr.ExploreFromXY(channel, secondSX, secondSY, 0 , RedSrhItrLimit);
                   detectRedBlockByBlue(DebugList);

                   if RedBlockExpr.isValidBlock() <> True then
                   begin//是可能的藍框
                      RedBlockExpr.clearBundValue();
                      isFouned := False;
                   end;

                   if isRBAreaValid() then
                   begin
                      isFouned := True;
                      BlueBlockExpr.drawBlockOnImg(drawTarImg, 1);
                      RedBlockExpr.drawBlockOnImg(drawTarImg, 1);
                   end;

                   //合法的話跳出迴圈直接結束
                   if isFouned then
                   begin
                      i := tarImg.Picture.Bitmap.Height-1;
                      j := tarImg.Picture.Bitmap.Width-1;
                      break;
                   end;
                   //BlueBlockExpr.clearBundValue();//如果藍框框被找到 但是不合法(不屬於眼鏡), 清除值
                   //Break; 跳過這個row直接搜尋下一個(如果眼鏡之前有一大塊藍色,這樣跳過的話,眼鏡永遠也不會被找到)
                   //不能把BlueBlock圈起來的區域歸0 會有害眼鏡也被切掉的風險
               end;

               j := j + xStep;
          end;
          i := i + yStep;
     end;
  end
  else
  begin
       showMessage('DrawTarImg是空的, 沒有影像畫框框, 無法執行紅藍眼鏡辨識!');
  end;
end;

function TEyePExplorer.isRBAreaValid(): Boolean;
//用於最後確認, 檢查 藍框跟紅框之間的各種比例關係是否合理(不合理應該就不是眼鏡, 回傳False)
var
   areaRatio, wRatio, hRatio: Double;
   bArea, rArea: Integer;
begin
   Result:=True;
   bArea := BlueBlockExpr.getArea();
   rArea := RedBlockExpr.getArea();
   if (bArea > 0) and (rArea > 0) then
   begin
       //檢查長寬比
       {wRatio := Max(BlueBlockExpr.getWidth(), RedBlockExpr.getWidth()) / Min(BlueBlockExpr.getWidth(), RedBlockExpr.getWidth());
       if wRatio >= WHRatioThr then
          Result := False;
       hRatio := Max(BlueBlockExpr.getHeight(), RedBlockExpr.getHeight()) / Min(BlueBlockExpr.getHeight(), RedBlockExpr.getHeight());
       if hRatio >= WHRatioThr then
          Result := False;}

       //簡查X軸 跟Z軸的重疊性
       if Result = True then
       begin
            //紅色的左邊 不能超過 藍色的右邊
            //且有一段距離
            if (RedBlockExpr.LeftBund - BlueBlockExpr.RightBund) < eyeInterval then
            begin
                 Result := False;
            end;
       end;

       //檢查是否為 正方形 或 (寬 > 長)的長方形
       if Result = True then
       begin
            //檢查藍
            if (BlueBlockExpr.getWidth() / BlueBlockExpr.getHeight()) < 1.35 then
            begin
                 Result := False;
            end;
            //檢查紅
            if (RedBlockExpr.getWidth() / RedBlockExpr.getHeight()) < 1.35 then
            begin
                 Result := False;     
            end;
       end; 
   
       //比對面積
       if Result = True then
       begin
           areaRatio := Max(bArea, rArea) / Min(bArea, rArea);
           if areaRatio >= AreaRatioThr then
           begin
                Result:=False;
           end;
       end;
   end
   else
   begin
        Result:=False;
   end;   
end;

function TEyePExplorer.detectRedBlockByBlue(DebugList:TListBox): Boolean;
//找到藍色框框後, 以藍色框框的資訊為依據, 往右找尋紅色框框
var
   isFound: Boolean;
   xIdx, xpl, yIdx, channel, srhLenLimit: Integer;
begin
     Result := False;
     
     if Assigned(BlueBlockExpr) = True then
     begin
          //Three way search
          channel := 0;
          srhLenLimit := Round(BlueBlockExpr.getWidth()/2.0); //search length limitation

          //DebugList.Items.Add('width:'+inttostr(BlueBlockExpr.getWidth()));

          xIdx:= BlueBlockExpr.RightBund + BlueBlockExpr.getWidth();
          //1 從中間的地方找
          yIdx := Round( (BlueBlockExpr.TopBund + BlueBlockExpr.BtmBund)/2.0 );

          isFound := False;
          if (xIdx < tarImg.Picture.Bitmap.Width-1) and (yIdx < tarImg.Picture.Bitmap.Height-1) then
              isFound := xAxisSearch(channel, xIdx, yIdx, srhLenLimit, RedBlockExpr, DebugList);
              
          if isFound = True then
             Result := True;
     end;
end;

function TEyePExplorer.xAxisSearch(channel, sX, sY, srhLenLimit: Integer; BlockExprHandler: TBlockExplore; DebugList:TListBox): Boolean;
{***********************************************************//
   由detectRedBlockByBlue(...)呼叫, 從指定的(sX,sY)座標開始,
   往右找尋指定顏色的像素(最遠找到座標(sX+srhLenLimit, sY)
   如果找到指定顏色的像素, 使用RedBlockExpr(紅框框搜尋器)
   找出整個框框, 如果成功找到框框, 回傳true, 否則回傳false 
************************************************************//}
var
  xIdx, xpl: Integer;
  tarP: PByteArray;
  xStep : Integer;
begin
    xStep := 2;

    Result := False;
    if (sY>=0) and (sY < tarImg.Picture.Bitmap.Height-1) then
      tarP := tarImg.Picture.Bitmap.ScanLine[ sY ];

    xIdx:= sX;
    while (xIdx <= sX+srhLenLimit) and (xIdx < tarImg.Picture.Bitmap.Width-1) do
    begin
         xpl := xIdx * 3 + channel;
         if tarP[xpl] > 127 then
         begin//成功找到red
              BlockExprHandler.ExploreFromXY(channel, xIdx, sY, 0 ,RedSrhItrLimit);
                   
              if BlockExprHandler.isValidBlock() and isRBAreaValid() then
              begin
                  //紅框search debug使用
                  //DebugList.Items.Add( '框有效範圍:('+inttostr(BlockExprHandler.LeftBund) + ',' + inttostr(BlockExprHandler.TopBund) + ') ~ (' + inttostr(BlockExprHandler.RightBund) + ',' + inttostr(BlockExprHandler.BtmBund) + ')');
                  Result := True;
                  break;
              end
              else
              begin//找到的不是正確的block
                  xpl := BlockExprHandler.RightBund;
                  BlockExprHandler.clearBundValue();
                  break;
              end;
         end;  

         xIdx := xIdx + xStep;
    end;
end;

end.
    