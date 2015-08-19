TBlockExplore = Class(TObject)  
    public
    DBG_DrawOnDBGC: Boolean;// draw on debug channel
    DBG_Channel: Integer;
    BlockVMinW, BlockVMinH, SrhChannelMinV: Integer;  //Block Valid Min Width and Height
    LeftBund, RightBund, TopBund, BtmBund  : Integer;
    Img: TImage;
    constructor Create(In_Img :TImage; In_BlockVMinW, In_BlockVMinH:Integer);

    procedure drawBlockOnImg(tarImg: TBitmap; subRate: Integer);

    function isValidBlock():Boolean;
    
    procedure ExploreFromXY(channel, oX, oY, itrCount, itrLimit: Integer);
    procedure HorizontalSearch(channel, oX, oY, itrCount, itrLimit:Integer);
    procedure VerticalSearch(channel, oX, oY, itrCount, itrLimit:Integer);
    procedure LTRDiagnal(channel, oX, oY, itrCount, itrLimit:Integer); // left to right
    procedure RTLDiagnal(channel, oX, oY, itrCount, itrLimit:Integer); // right to left
    
    procedure searchDebugging(pSCLine: PByteArray; xIdx:Integer);
    procedure drawDebugLine();
    
    procedure setFourBundToValid(tarImg: TBitmap);
    procedure changeImg(New_Img: TImage);
    
    procedure clearBundValue();
    function getWidth(): Integer;
    function getHeight(): Integer;
    function getArea(): Integer;
end;
