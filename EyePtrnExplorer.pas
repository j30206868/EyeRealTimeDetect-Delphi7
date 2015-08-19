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
        //��Pattern
        procedure searchEyePattern(DebugList:TListBox);
        //�H�Ůت���m ��������(�ϥΫe�����T�{BlueBlockExpr�w�g�Q��l�ƤF)
        function detectRedBlockByBlue(DebugList:TListBox): Boolean;
        //BlockExprHandler := TBlockExplore.Create(tarImg) �ϥΫe�n����l��handler
        function xAxisSearch(channel, sX, sY, srhLenLimit: Integer; BlockExprHandler: TBlockExplore; DebugList:TListBox): Boolean;
        function isRBAreaValid(): Boolean;
    end;

implementation

{$i BlockExplore.inc}

constructor TEyePExplorer.Create(IN_Img: TImage; IN_DrawTarImg: TBitmap);
//�غc��: �]�w�Ҧ���threshold���ܼ�
begin
     AreaRatioThr := 1.6; //�Ůظ���� ���n��(�j/�p) �̦h�t�h�֭� �~��X�k
     WHRatioThr := 1.7;   //�Ůظ���� ���e��(�j/�p) �̦h�t�h�֭� �~��X�k
     //�خس̤p�����e����(��SrhXYStep����)   
     BlockVMinW := 10;
     BlockVMinH := 5;
     //�j�M���I�� �@�������Z��(�o�i)
     SrhXStep   := 10;
     SrhYStep   := 5;
     BlueSrhItrLimit := 1;
     RedSrhItrLimit := 1; //��خخ�, �Ϯg���̤j���� 

     //�Ⲵ�خت��Z��
     eyeInterval := 10;

     tarImg := IN_Img;            //�j�M�ϥΪ��v��(�q�`�O����subsample�᪺�v��)
     drawTarImg := IN_DrawTarImg; //�e�خ� �ɪ� �ؼмv��(�q�`�O���)
     RedBlockExpr  := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//����خ� �ϥΪ��خطj�M��
     BlueBlockExpr := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//���Ůخ� �ϥΪ��خطj�M��
end;

constructor TEyePExplorer.Create(IN_Img: TImage);
//�غc��: �]�w�Ҧ���threshold���ܼ�
begin
     AreaRatioThr := 1.2; //�Ůظ���� ���n��(�j/�p) �̦h�t�h�֭� �~��X�k
     WHRatioThr := 1.5;   //�Ůظ���� ���e��(�j/�p) �̦h�t�h�֭� �~��X�k
     //�خس̤p�����e����(��SrhXYStep����)   
     BlockVMinW := 20;
     BlockVMinH := 10; 
     //�j�M���I�� �@�������Z��(�o�i) 
     SrhXStep   := 20;
     SrhYStep   := 10;
     RedSrhItrLimit := 1; //��خخ�, �Ϯg���̤j����
     BlueSrhItrLimit := 1;

     tarImg := IN_Img;            //�j�M�ϥΪ��v��(�q�`�O����subsample�᪺�v��)
     RedBlockExpr  := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//����خ� �ϥΪ��خطj�M��
     BlueBlockExpr := TBlockExplore.Create(tarImg, BlockVMinW, BlockVMinH);//���Ůخ� �ϥΪ��خطj�M��
end;

procedure TEyePExplorer.searchEyePattern(DebugList:TListBox);
{***********************************************************//
       �j�MTarImg(�H�C��Step��@�����覡), ���L��i��, 
            �����X�Ĥ@�ӲŦX���Ų��誺pattern 
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

     //�]�w �j�M�� �@�����X��(�o�i, �קK��h�����n���j�M) 
     xStep := SrhXStep;
     yStep := SrhYStep;

     isFouned := False;
   
     channel := 0;//�Ū�channel�O0
     i := 5;
     //��MtarImg�������I
     while i < tarImg.Picture.Bitmap.Height-6 do
     begin
          tarP := tarImg.Picture.Bitmap.Scanline[i];

          j:=10;
          while j < tarImg.Picture.Bitmap.Width-11 do
          begin
               xI := j * 3 + channel;
               //�ˬd�O�_�����I
               if tarP[xI] > 0 then
               begin//������I�F
                   BlueBlockExpr.clearBundValue();
                   RedBlockExpr.clearBundValue();
                   BlueBlockExpr.ExploreFromXY(channel, j, i, 0 ,BlueSrhItrLimit);//�q���I ��X�Ůخ�
    
                   //check if the block is valid(�ݬ�block�����e ���S���i��O���誺�Ů� �O�_�Ӥp�i�H��������?)
                   if BlueBlockExpr.isValidBlock() <> True then
                   begin//�O�i�઺�Ů�
                      BlueBlockExpr.clearBundValue();
                      isFouned := False;
                   end;

                   //�k��خظ��i���쪺�a��
                   //secondSX := BlueBlockExpr.RightBund + (BlueBlockExpr.RightBund - BlueBlockExpr.LeftBund);
                   //secondSY := Round( (BlueBlockExpr.TopBund + BlueBlockExpr.BtmBund)/2.0 );
                   //RedBlockExpr.ExploreFromXY(channel, secondSX, secondSY, 0 , RedSrhItrLimit);
                   detectRedBlockByBlue(DebugList);

                   if RedBlockExpr.isValidBlock() <> True then
                   begin//�O�i�઺�Ů�
                      RedBlockExpr.clearBundValue();
                      isFouned := False;
                   end;

                   if isRBAreaValid() then
                   begin
                      isFouned := True;
                      BlueBlockExpr.drawBlockOnImg(drawTarImg, 1);
                      RedBlockExpr.drawBlockOnImg(drawTarImg, 1);
                   end;

                   //�X�k���ܸ��X�j�骽������
                   if isFouned then
                   begin
                      i := tarImg.Picture.Bitmap.Height-1;
                      j := tarImg.Picture.Bitmap.Width-1;
                      break;
                   end;
                   //BlueBlockExpr.clearBundValue();//�p�G�ŮخسQ��� ���O���X�k(���ݩ���), �M����
                   //Break; ���L�o��row�����j�M�U�@��(�p�G���褧�e���@�j���Ŧ�,�o�˸��L����,����û��]���|�Q���)
                   //�����BlueBlock��_�Ӫ��ϰ��k0 �|���`����]�Q���������I
               end;

               j := j + xStep;
          end;
          i := i + yStep;
     end;
  end
  else
  begin
       showMessage('DrawTarImg�O�Ū�, �S���v���e�خ�, �L�k������Ų������!');
  end;
end;

function TEyePExplorer.isRBAreaValid(): Boolean;
//�Ω�̫�T�{, �ˬd �Ůظ���ؤ������U�ؤ�����Y�O�_�X�z(���X�z���ӴN���O����, �^��False)
var
   areaRatio, wRatio, hRatio: Double;
   bArea, rArea: Integer;
begin
   Result:=True;
   bArea := BlueBlockExpr.getArea();
   rArea := RedBlockExpr.getArea();
   if (bArea > 0) and (rArea > 0) then
   begin
       //�ˬd���e��
       {wRatio := Max(BlueBlockExpr.getWidth(), RedBlockExpr.getWidth()) / Min(BlueBlockExpr.getWidth(), RedBlockExpr.getWidth());
       if wRatio >= WHRatioThr then
          Result := False;
       hRatio := Max(BlueBlockExpr.getHeight(), RedBlockExpr.getHeight()) / Min(BlueBlockExpr.getHeight(), RedBlockExpr.getHeight());
       if hRatio >= WHRatioThr then
          Result := False;}

       //²�dX�b ��Z�b�����|��
       if Result = True then
       begin
            //���⪺���� ����W�L �Ŧ⪺�k��
            //�B���@�q�Z��
            if (RedBlockExpr.LeftBund - BlueBlockExpr.RightBund) < eyeInterval then
            begin
                 Result := False;
            end;
       end;

       //�ˬd�O�_�� ����� �� (�e > ��)�������
       if Result = True then
       begin
            //�ˬd��
            if (BlueBlockExpr.getWidth() / BlueBlockExpr.getHeight()) < 1.35 then
            begin
                 Result := False;
            end;
            //�ˬd��
            if (RedBlockExpr.getWidth() / RedBlockExpr.getHeight()) < 1.35 then
            begin
                 Result := False;     
            end;
       end; 
   
       //��ﭱ�n
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
//����Ŧ�خث�, �H�Ŧ�خت���T���̾�, ���k��M����خ�
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
          //1 �q�������a���
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
   ��detectRedBlockByBlue(...)�I�s, �q���w��(sX,sY)�y�ж}�l,
   ���k��M���w�C�⪺����(�̻����y��(sX+srhLenLimit, sY)
   �p�G�����w�C�⪺����, �ϥ�RedBlockExpr(���خطj�M��)
   ��X��Ӯخ�, �p�G���\���خ�, �^��true, �_�h�^��false 
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
         begin//���\���red
              BlockExprHandler.ExploreFromXY(channel, xIdx, sY, 0 ,RedSrhItrLimit);
                   
              if BlockExprHandler.isValidBlock() and isRBAreaValid() then
              begin
                  //����search debug�ϥ�
                  //DebugList.Items.Add( '�ئ��Ľd��:('+inttostr(BlockExprHandler.LeftBund) + ',' + inttostr(BlockExprHandler.TopBund) + ') ~ (' + inttostr(BlockExprHandler.RightBund) + ',' + inttostr(BlockExprHandler.BtmBund) + ')');
                  Result := True;
                  break;
              end
              else
              begin//��쪺���O���T��block
                  xpl := BlockExprHandler.RightBund;
                  BlockExprHandler.clearBundValue();
                  break;
              end;
         end;  

         xIdx := xIdx + xStep;
    end;
end;

end.
    