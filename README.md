# 静止画・動画ファイル解析ツール agalia

## サポートフォーマット

* ASF (Advanced Systems Format)
  * .asf, .wmv, etc.
* BMP
* DICOM
* ICC Profile
* ISO base media format
  * .mp4, .mov, etc.
* JPEG (Exif JPEG 含む)
* MPEG-2 program stream
  * .mpg, .m2v, .vob, etc.
* PNG
* RIFF (Resource Interchange File Format)
  * .avi, .wav, webp, etc.
* TIFF (Exif TIFF、BigTIFF、DNG 含む)

## 基本的な使い方

### 起動方法
1. agalia.exe を実行し、画像ファイルをドロップする

### DICOM の Private タグの名称を表示させる方法
1. agalia.exe と同じフォルダにある dcm_private.xml を編集して上書き保存
1. agalia.exe に DICOM ファイルをドロップする

### MJPEG の AVI ファイルに含まれる JPEG データの解析
1. agalia.exe に AVI ファイルをドロップ
1. movi の 00dc を探して、data が FF,D8 で始まっているはずなので、それを右クリックして Analyze

### Exif JPEG に含まれる Exif データの解析
1. agalia.exe に JPEG ファイルをドロップ
1. APP1 の identifier=Exif を探して、それを右クリックして Analyze

### 巨大ファイル(2GB以上)を読み込む方法
1. x64版の agalia.exe にファイルをドロップ
1. 解析に時間がかかりすぎる場合はプログレスダイアログで[キャンセル]

### 解析結果をExcelに貼り付ける方法（１）
1. agalia.exe で解析対象ファイルを開く
1. Ctrl+Aで全選択、Ctrl+Cでコピー
1. Excelでシート全選択して表示形式を文字列にしてからペースト

### 解析結果をExcelに貼り付ける方法（２）
1. コマンドラインで agalia hoge.dcm /out:a.txt
1. Excelでa.txtを開き、テキストファイルウィザードでタブ区切り、すべての列のデータ形式を文字列にして[完了]

### ファイルフォーマットを指定して開く方法
1. agalia.exe で[File]-[Open...]でファイルオープンダイアログを開く
1. 右下の[Format]でフォーマットを選択してファイルを開く


## 参考文献

### DICOM

* [DICOM Standard](http://dicom.nema.org/standard.html)
  * DICOM PS3.5 2016e - Data Structures and Encoding
  * DICOM PS3.6 2016d - Data Dictionary
  * DICOM PS3.10 2016e - Media Storage and File Format for Media Interchange
* [標準DICOMヘッダセット](http://www.jira-net.or.jp/dicom/dicom_data_01.html)

### JPEG

* [Digital compression and coding of continuous-tone still images - Requirements and guidelines (ITU-T T.81 09/92)](http://www.w3.org/Graphics/JPEG/itu-t81.pdf)
* [Digital compression and coding of continuous-tone still images: JPEG File Interchange Format (ITU-T T.871 05/2011)](http://www.itu.int/rec/T-REC-T.871)

### TIFF

* [TIFF 6.0 Specification](https://www.adobe.io/open/standards/TIFF.html)
  * TIFF 6.0 Specification
  * Adobe PageMaker® 6.0 TIFF Technical Notes
  * Adobe Photoshop® TIFF Technical Notes
* [The BigTIFF File Format Proposal](http://www.awaresystems.be/imaging/tiff/bigtiff.html)

### Exif

* [CIPA規格類](http://www.cipa.jp/std/std-sec_j.html)
  * デジタルスチルカメラ用画像ファイルフォーマット規格 Exif 2.31
  * カメラファイルシステム規格DCF2.0（2010年版）

### QuickTime Movie

* [QuickTime File Format Specification 2015-02-14](https://web.archive.org/web/20150908100905/https://developer.apple.com/library/mac/documentation/QuickTime/QTFF/qtff.pdf)
* [QuickTime Atoms and Resources Reference 2006-05-23](https://developer.apple.com/library/content/documentation/QuickTime/Reference/QTRef_AtomsResources/QTRef_AtomsResources.pdf)


### MPEG-2

* [Generic coding of moving pictures and associated audio information: Systems (ITU-T H.222.0 06/12)](https://www.itu.int/rec/T-REC-H.222.0-201206-S/en)

### MPEG-4

* [ISO base media file format (ISO/IEC 14496-12:2015)](http://standards.iso.org/ittf/PubliclyAvailableStandards/c068960_ISO_IEC_14496-12_2015.zip)
* [MP4 file form (ISO/IEC 14496-14:2003)](http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=38538&commid=45316)

### ASF

* [Advanced Systems Format (ASF) Specification (Revision 01.20.06)](https://web.archive.org/web/20121003052515/http://www.microsoft.com/en-us/download/details.aspx?displaylang=en&id=14995)

### Bitmap

* [Bitmap Storage](https://msdn.microsoft.com/en-us/library/dd183391(v=vs.85).aspx)

### RIFF

* [Resource Interchange File Format Services](https://msdn.microsoft.com/en-us/library/dd798636(v=vs.85).aspx)
* [AVI RIFF File Reference](https://msdn.microsoft.com/en-us/library/dd318189(v=vs.85).aspx)

### PNG

* [Portable Network Graphics (PNG) Specification (Second Edition)](https://www.w3.org/TR/PNG/)
* [Extensions to the PNG 1.2 Specification, Version 1.5.0](http://ftp-osl.osuosl.org/pub/libpng/documents/pngext-1.5.0.html)

### ICC Profile
* [Specification ICC.1:2022 (Profile version 4.4.0.0)](https://www.color.org/specification/ICC.1-2022-05.pdf)
* [Specification ICC.1:2001-04](https://www.color.org/ICC_Minor_Revision_for_Web.pdf)

## テストデータ

* [TIFF (ITU-T T.24)](https://www.itu.int/wftp3/Public/t/testsignal/GenImage/T024/)
* [JPEG (ITU-T T.83)](https://www.itu.int/wftp3/Public/t/testsignal/SpeImage/T083v1_0/)
* [BigTIFF example files](http://www.awaresystems.be/imaging/tiff/bigtiff.html#samples)
* [QuickTime：サンプルファイル](https://support.apple.com/ja-jp/HT201549)
* DICOM (NEMA DICOM CD 1997) (ftp://dicom.offis.uni-oldenburg.de/pub/dicom/images/)
* DICOM (NEMA WG12) (ftp://medical.nema.org/medical/dicom/datasets/wg12/)
* [DICOM (OsiriX)](http://www.osirix-viewer.com/resources/dicom-image-library/)
* [DICOM画像ファイル (JIRA)](http://www.jira-net.or.jp/dicom/dicom_data_01_02.html)
* [DICOM画像ファイル2012](http://www.jira-net.or.jp/dicom/dicom_data_01_03.html)

## 連絡先

[Twitter](https://twitter.com/solomon_qwe)

## License

Copyright (c) Solomon QWE. All rights reserved.

Licensed under the [MIT](LICENSE.txt) License.

## Third-Party Licenses

This project includes third-party software components, each of which is licensed under its respective open source license. The details of each third-party license are provided below.

---

### OpenJPEG
```
/*
 * The copyright in this software is being made available under the 2-clauses 
 * BSD License, included below. This software may be subject to other third 
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2003-2009, Francois-Olivier Devaux
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2011-2012, Centre National d'Etudes Spatiales (CNES), France 
 * Copyright (c) 2012, CS Systemes d'Information, France
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
 ```