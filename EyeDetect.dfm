object Form1: TForm1
  Left = 154
  Top = 126
  Width = 928
  Height = 571
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object VLImageDisplay1: TVLImageDisplay
    Left = 8
    Top = 0
    Width = 641
    Height = 481
    AutoSize = True
    Stretch = True
    InputPin.Form = Form1
    InputPin.SourcePin = Form1.VLGenericFilter1.OutputPin
    AspectRatio.Width = 4
    AspectRatio.Height = 3
  end
  object DebugList: TListBox
    Left = 680
    Top = 24
    Width = 217
    Height = 457
    ItemHeight = 13
    TabOrder = 1
  end
  object VLGenericFilter1: TVLGenericFilter
    OutputPin.Form = Form1
    OutputPin.SinkPins = (
      Form1.VLImageDisplay1.InputPin)
    InputPin.Form = Form1
    InputPin.SourcePin = Form1.VLDSCapture1.OutputPin
    OnStart = VLGenericFilter1Start
    OnProcessData = VLGenericFilter1ProcessData
    Left = 80
    Top = 72
  end
  object VLDSCapture1: TVLDSCapture
    OutputPin.Form = Form1
    OutputPin.SinkPins = (
      Form1.VLGenericFilter1.InputPin)
    FrameRate.Rate = 30.000000000000000000
    FrameRate.VariableRate = False
    VideoFormat = vfRGB24
    TVTuner.Format = tvfCurrent
    TVTuner.InputType = tviCurrent
    TVTuner.Mode = tvmCurrent
    DVResolution = dvrUseCurrent
    VideoCaptureDevice.AlternativeDevices = <>
    AudioCaptureDevice.AlternativeDevices = <>
    VideoSources.LinkRelated = True
    VideoSources.Selections = ()
    AudioSources.LinkRelated = True
    AudioSources.Selections = ()
    Adjustment.Brightness.Mode = amUseCurrent
    Adjustment.Brightness.Value = 0
    Adjustment.Contrast.Mode = amUseCurrent
    Adjustment.Contrast.Value = 0
    Adjustment.Hue.Mode = amUseCurrent
    Adjustment.Hue.Value = 0
    Adjustment.Saturation.Mode = amUseCurrent
    Adjustment.Saturation.Value = 0
    Adjustment.Sharpness.Mode = amUseCurrent
    Adjustment.Sharpness.Value = 0
    Adjustment.Gamma.Mode = amUseCurrent
    Adjustment.Gamma.Value = 0
    Adjustment.ColorEnable.Mode = amUseCurrent
    Adjustment.ColorEnable.Value = False
    Adjustment.WhiteBalance.Mode = amUseCurrent
    Adjustment.WhiteBalance.Value = 0
    Adjustment.BacklightCompensation.Mode = amUseCurrent
    Adjustment.BacklightCompensation.Value = False
    Adjustment.Gain.Mode = amUseCurrent
    Adjustment.Gain.Value = 0
    CameraControl.Pan.Mode = amUseCurrent
    CameraControl.Pan.Value = 0
    CameraControl.Tilt.Mode = amUseCurrent
    CameraControl.Tilt.Value = 0
    CameraControl.Roll.Mode = amUseCurrent
    CameraControl.Roll.Value = 0
    CameraControl.Zoom.Mode = amUseCurrent
    CameraControl.Zoom.Value = 0
    CameraControl.Exposure.Mode = amUseCurrent
    CameraControl.Exposure.Value = 0
    CameraControl.Iris.Mode = amUseCurrent
    CameraControl.Iris.Value = 0
    CameraControl.Focus.Mode = amUseCurrent
    CameraControl.Focus.Value = 0
    Graph.AdditionalFilters = <>
    Left = 32
    Top = 64
  end
end
