#pragma once
#include "AL/Common.hpp"

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types

namespace AL::Serialization::HTTP
{
	constexpr const String::Char CONTENT_TYPE_AAC[]     = "";
	constexpr const String::Char CONTENT_TYPE_ABW[]     = "";
	constexpr const String::Char CONTENT_TYPE_ARC[]     = "";
	constexpr const String::Char CONTENT_TYPE_AVIF[]    = "";
	constexpr const String::Char CONTENT_TYPE_AVI[]     = "";
	constexpr const String::Char CONTENT_TYPE_AZW[]     = "";
	constexpr const String::Char CONTENT_TYPE_BIN[]     = "";
	constexpr const String::Char CONTENT_TYPE_BMP[]     = "";
	constexpr const String::Char CONTENT_TYPE_BZ[]      = "";
	constexpr const String::Char CONTENT_TYPE_BZ2[]     = "";
	constexpr const String::Char CONTENT_TYPE_CDA[]     = "";
	constexpr const String::Char CONTENT_TYPE_CSH[]     = "";
	constexpr const String::Char CONTENT_TYPE_CSS[]     = "";
	constexpr const String::Char CONTENT_TYPE_CSV[]     = "";
	constexpr const String::Char CONTENT_TYPE_DOC[]     = "";
	constexpr const String::Char CONTENT_TYPE_DOCX[]    = "";
	constexpr const String::Char CONTENT_TYPE_EOT[]     = "";
	constexpr const String::Char CONTENT_TYPE_EPUB[]    = "";
	constexpr const String::Char CONTENT_TYPE_GZ[]      = "";
	constexpr const String::Char CONTENT_TYPE_GIF[]     = "";
	constexpr const String::Char CONTENT_TYPE_HTM[]     = "text/html";
	constexpr const String::Char CONTENT_TYPE_HTML[]    = "text/html";
	constexpr const String::Char CONTENT_TYPE_ICO[]     = "";
	constexpr const String::Char CONTENT_TYPE_ICS[]     = "";
	constexpr const String::Char CONTENT_TYPE_JAR[]     = "";
	constexpr const String::Char CONTENT_TYPE_JPG[]     = "";
	constexpr const String::Char CONTENT_TYPE_JPEG[]    = "";
	constexpr const String::Char CONTENT_TYPE_JS[]      = "";
	constexpr const String::Char CONTENT_TYPE_JSON[]    = "application/json";
	constexpr const String::Char CONTENT_TYPE_JSON_LD[] = "";
	constexpr const String::Char CONTENT_TYPE_MID[]     = "";
	constexpr const String::Char CONTENT_TYPE_MIDI[]    = "";
	constexpr const String::Char CONTENT_TYPE_MJS[]     = "";
	constexpr const String::Char CONTENT_TYPE_MP3[]     = "";
	constexpr const String::Char CONTENT_TYPE_MP4[]     = "";
	constexpr const String::Char CONTENT_TYPE_MPEG[]    = "";
	constexpr const String::Char CONTENT_TYPE_MPKG[]    = "";
	constexpr const String::Char CONTENT_TYPE_ODP[]     = "";
	constexpr const String::Char CONTENT_TYPE_ODS[]     = "";
	constexpr const String::Char CONTENT_TYPE_ODT[]     = "";
	constexpr const String::Char CONTENT_TYPE_OGA[]     = "";
	constexpr const String::Char CONTENT_TYPE_OGV[]     = "";
	constexpr const String::Char CONTENT_TYPE_OGX[]     = "";
	constexpr const String::Char CONTENT_TYPE_OPUS[]    = "";
	constexpr const String::Char CONTENT_TYPE_OTF[]     = "";
	constexpr const String::Char CONTENT_TYPE_PNG[]     = "";
	constexpr const String::Char CONTENT_TYPE_PDF[]     = "";
	constexpr const String::Char CONTENT_TYPE_PHP[]     = "";
	constexpr const String::Char CONTENT_TYPE_PPT[]     = "";
	constexpr const String::Char CONTENT_TYPE_PPTX[]    = "";
	constexpr const String::Char CONTENT_TYPE_RAR[]     = "";
	constexpr const String::Char CONTENT_TYPE_RTF[]     = "";
	constexpr const String::Char CONTENT_TYPE_SH[]      = "";
	constexpr const String::Char CONTENT_TYPE_SVG[]     = "";
	constexpr const String::Char CONTENT_TYPE_TAR[]     = "";
	constexpr const String::Char CONTENT_TYPE_TIF[]     = "";
	constexpr const String::Char CONTENT_TYPE_TIFF[]    = "";
	constexpr const String::Char CONTENT_TYPE_TS[]      = "";
	constexpr const String::Char CONTENT_TYPE_TTF[]     = "";
	constexpr const String::Char CONTENT_TYPE_TXT[]     = "text/plain";
	constexpr const String::Char CONTENT_TYPE_VSD[]     = "";
	constexpr const String::Char CONTENT_TYPE_WAV[]     = "audio/wav";
	constexpr const String::Char CONTENT_TYPE_WEBA[]    = "";
	constexpr const String::Char CONTENT_TYPE_WEBM[]    = "";
	constexpr const String::Char CONTENT_TYPE_WEBP[]    = "";
	constexpr const String::Char CONTENT_TYPE_WOFF[]    = "";
	constexpr const String::Char CONTENT_TYPE_WOFF2[]   = "";
	constexpr const String::Char CONTENT_TYPE_XHTML[]   = "";
	constexpr const String::Char CONTENT_TYPE_XLS[]     = "";
	constexpr const String::Char CONTENT_TYPE_XLSX[]    = "";
	constexpr const String::Char CONTENT_TYPE_XML[]     = "application/xml";
	constexpr const String::Char CONTENT_TYPE_XUL[]     = "application/vnd.mozilla.xul+xml";
	constexpr const String::Char CONTENT_TYPE_ZIP[]     = "application/zip";
	constexpr const String::Char CONTENT_TYPE_3GP[]     = "audio/3gpp";
	constexpr const String::Char CONTENT_TYPE_3GPV[]    = "video/3gpp";
	constexpr const String::Char CONTENT_TYPE_3G2[]     = "audio/3gpp2";
	constexpr const String::Char CONTENT_TYPE_3G2V[]    = "video/3gpp2";
	constexpr const String::Char CONTENT_TYPE_7Z[]      = "application/x-7z-compressed";
}
