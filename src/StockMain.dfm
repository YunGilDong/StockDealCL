object StockMainF: TStockMainF
  Left = 0
  Top = 0
  Caption = 'StockMainF'
  ClientHeight = 363
  ClientWidth = 826
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lbCommSt: TLabel
    Left = 456
    Top = 45
    Width = 8
    Height = 13
    Caption = '--'
  end
  object Label1: TLabel
    Left = 24
    Top = 104
    Width = 54
    Height = 13
    Caption = 'RECV INFO'
  end
  object Label2: TLabel
    Left = 417
    Top = 104
    Width = 54
    Height = 13
    Caption = 'SEND INFO'
  end
  object btnEcho: TButton
    Left = 16
    Top = 33
    Width = 75
    Height = 25
    Caption = 'Echo'
    TabOrder = 0
    OnClick = btnEchoClick
  end
  object Button2: TButton
    Left = 328
    Top = 33
    Width = 75
    Height = 25
    Caption = 'Button2'
    TabOrder = 1
  end
  object Button3: TButton
    Left = 216
    Top = 33
    Width = 75
    Height = 25
    Caption = 'Button3'
    TabOrder = 2
  end
  object lbLog: TListBox
    Left = 0
    Top = 132
    Width = 395
    Height = 223
    ItemHeight = 13
    TabOrder = 3
  end
  object lbSendLog: TListBox
    Left = 417
    Top = 128
    Width = 395
    Height = 223
    ItemHeight = 13
    TabOrder = 4
  end
  object tmTcpSt: TTimer
    Enabled = False
    OnTimer = tmTcpStTimer
    Left = 640
    Top = 32
  end
end
