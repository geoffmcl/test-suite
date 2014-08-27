
// test-codeset.c

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

#ifndef nl_item
#define nl_item int
#endif

#define DEF_CODESET 1000

#define C_CODESET "UTF-8"     /* Return this as the default encoding */
//#define C_CODESET "US-ASCII"     /* Return this as the encoding of the
//				  * C/POSIX locale. Could as well one day
//				  * become "UTF-8". */

#define digit(x) ((x) >= '0' && (x) <= '9')

static char buf[16];

// from : http://msdn.microsoft.com/en-US/goglobal/bb896001.aspx
// 0x201A,bs-Cyrl-BA,Bosnian(Cyrillic, Bosnia and Herzegovina),Bosnian,1251,855,BIH,BSC

typedef struct tagLCID2NAMES {
    int LCID;
    const char *nm;
    const char *co;
    const char *lang;
    const char *other;
    int ansiicp;
    int oemcp;
    const char *rega;
    const char *langa;
} LCID2NAMES, *PLCID2NAMES;

LCID2NAMES LCID2Names[] = {
//    { 0x0036, "af", "Afrikaans", "Afrikaans", "Afrikaans", 1252, 850, "ZAF", "AFK" },
    { 0x0036, "af", "Afrikaans", "Afrikaans", "Afrikaans", 1252, 850, "ZAF", "AFK" },
    { 0x0436, "af-ZA", "Afrikaans(South Africa)", "Afrikaans", "Afrikaans (Suid Afrika)", 1252, 850, "ZAF", "AFK" },
    { 0x001C, "sq", "Albanian", "Albanian", "shqipe", 1250, 852, "ALB", "SQI" },
    { 0x041C, "sq-AL", "Albanian(Albania)", "Albanian", "shqipe (Shqip�ria)", 1250, 852, "ALB", "SQI" },
    { 0x0484, "gsw-FR", "Alsatian(France)", "Alsatian", "Els�ssisch (Fr�nkrisch)", 1252, 850, "FRA", "GSW" },
    { 0x045E, "am-ET", "Amharic(Ethiopia)", "Amharic", "???? (?????)", 0, 1, "eth", "AMH" },
    { 0x0001, "ar", "Arabic?", "Arabic", "????????", 1256, 720, "SAU", "ARA" },
    { 0x1401, "ar-DZ", "Arabic(Algeria)?", "Arabic", "??????? (???????)?", 1256, 720, "DZA", "ARG" },
    { 0x3C01, "ar-BH", "Arabic(Bahrain)?", "Arabic", "??????? (???????)?", 1256, 720, "BHR", "ARH" },
    { 0x0C01, "ar-EG", "Arabic(Egypt)?", "Arabic", "??????? (???)?", 1256, 720, "EGY", "ARE" },
    { 0x0801, "ar-IQ", "Arabic(Iraq)?", "Arabic", "??????? (??????)?", 1256, 720, "IRQ", "ARI" },
    { 0x2C01, "ar-JO", "Arabic(Jordan)?", "Arabic", "??????? (??????)?", 1256, 720, "JOR", "ARJ" },
    { 0x3401, "ar-KW", "Arabic(Kuwait)?", "Arabic", "??????? (??????)?", 1256, 720, "KWT", "ARK" },
    { 0x3001, "ar-LB", "Arabic(Lebanon)?", "Arabic", "??????? (?????)?", 1256, 720, "LBN", "ARB" },
    { 0x1001, "ar-LY", "Arabic(Libya)?", "Arabic", "??????? (?????)?", 1256, 720, "LBY", "ARL" },
    { 0x1801, "ar-MA", "Arabic(Morocco)?", "Arabic", "??????? (??????? ????????)?", 1256, 720, "MAR", "ARM" },
    { 0x2001, "ar-OM", "Arabic(Oman)?", "Arabic", "??????? (????)?", 1256, 720, "OMN", "ARO" },
    { 0x4001, "ar-QA", "Arabic(Qatar)?", "Arabic", "??????? (???)?", 1256, 720, "QAT", "ARQ" },
    { 0x0401, "ar-SA", "Arabic(Saudi Arabia)?", "Arabic", "??????? (??????? ??????? ????????)?", 1256, 720, "SAU", "ARA" },
    { 0x2801, "ar-SY", "Arabic(Syria)?", "Arabic", "??????? (?????)?", 1256, 720, "SYR", "ARS" },
    { 0x1C01, "ar-TN", "Arabic(Tunisia)?", "Arabic", "??????? (????)?", 1256, 720, "TUN", "ART" },
    { 0x3801, "ar-AE", "Arabic(U.A.E.)?", "Arabic", "??????? (???????? ??????? ???????)?", 1256, 720, "ARE", "ARU" },
    { 0x2401, "ar-YE", "Arabic(Yemen)?", "Arabic", "??????? (?????)?", 1256, 720, "YEM", "ARY" },
    { 0x002B, "hy", "Armenian", "Armenian", "???????", 0, 1, "ARM", "HYE" },
    { 0x042B, "hy-AM", "Armenian(Armenia)", "Armenian", "??????? (????????)", 0, 1, "ARM", "HYE" },
    { 0x044D, "as-IN", "Assamese(India)", "Assamese", "?????? (????)", 0, 1, "IND", "ASM" },
    { 0x002C, "az", "Azeri", "Azeri", "(Latin) Az?rbaycan�ili", 1254, 857, "AZE", "AZE" },
    { 0x082C, "az-Cyrl-AZ", "Azeri(Cyrillic, Azerbaijan)", "Azeri", "(Cyrillic) ?????????? (??????????)", 1251, 866, "AZE", "AZE" },
    { 0x042C, "az-Latn-AZ", "Azeri(Latin, Azerbaijan)", "Azeri", "(Latin) Az?rbaycan�ili (Az?rbaycanca)", 1254, 857, "AZE", "AZE" },
    { 0x046D, "ba-RU", "Bashkir(Russia)", "Bashkir", "??????? (??????)", 1251, 866, "RUS", "BAS" },
    { 0x002D, "eu", "Basque", "Basque", "euskara", 1252, 850, "ESP", "EUQ" },
    { 0x042D, "eu-ES", "Basque(Basque)", "Basque", "euskara (euskara)", 1252, 850, "ESP", "EUQ" },
    { 0x0023, "be", "Belarusian", "Belarusian", "?????????", 1251, 866, "BLR", "BEL" },
    { 0x0423, "be-BY", "Belarusian(Belarus)", "Belarusian", "????????? (????????)", 1251, 866, "BLR", "BEL" },
    { 0x0845, "bn-BD", "Bengali(Bangladesh)", "Bengali", "????? (?????)", 0, 1, "BDG", "BNG" },
    { 0x0445, "bn-IN", "Bengali(India)", "Bengali", "????? (????)", 0, 1, "IND", "BNG" },
    { 0x201A, "bs-Cyrl-BA", "Bosnian(Cyrillic, Bosnia and Herzegovina)", "Bosnian", "(Cyrillic) ???????? (????? ? ???????????)", 1251, 855, "BIH", "BSC" },
    { 0x141A, "bs-Latn-BA", "Bosnian(Latin, Bosnia and Herzegovina)", "Bosnian", "(Latin) bosanski (Bosna i Hercegovina)", 1250, 852, "BIH", "BSB" },
    { 0x047E, "br-FR", "Breton(France)", "Breton", "brezhoneg (Fra�s)", 1252, 850, "FRA", "BRE" },
    { 0x0002, "bg", "Bulgarian", "Bulgarian", "?????????", 1251, 866, "BGR", "BGR" },
    { 0x0402, "bg-BG", "Bulgarian(Bulgaria)", "Bulgarian", "????????? (????????)", 1251, 866, "BGR", "BGR" },
    { 0x0003, "ca", "Catalan", "Catalan", "catal�", 1252, 850, "ESP", "CAT" },
    { 0x0403, "ca-ES", "Catalan(Catalan)", "Catalan", "catal� (catal�)", 1252, 850, "ESP", "CAT" },
    { 0x0C04, "zh-HK", "Chinese(Hong Kong S.A.R.)", "Chinese", "??(???????)", 950, 950, "HKG", "ZHH" },
    { 0x1404, "zh-MO", "Chinese(Macao S.A.R.)", "Chinese", "??(???????)", 950, 950, "MCO", "ZHM" },
    { 0x0804, "zh-CN", "Chinese(People's Republic of China)", "Chinese", "??(???????)", 936, 936, "CHN", "CHS" },
    { 0x0004, "zh-Hans", "Chinese(Simplified)", "Chinese", "??(??)", 936, 936, "CHN", "CHS" },
    { 0x1004, "zh-SG", "Chinese(Singapore)", "Chinese", "??(???)", 936, 936, "SGP", "ZHI" },
    { 0x0404, "zh-TW", "Chinese(Taiwan)", "Chinese", "??(??)", 950, 950, "TWN", "CHT" },
    { 0x7C04, "zh-Hant", "Chinese(Traditional)", "Chinese", "??(??)", 950, 950, "TWN", "CHT" },
    { 0x0483, "co-FR", "Corsican(France)", "Corsican", "Corsu (France)", 1252, 850, "FRA", "COS" },
    { 0x001A, "hr", "Croatian", "Croatian", "hrvatski", 1250, 852, "HRV", "HRV" },
    { 0x041A, "hr-HR", "Croatian(Croatia)", "Croatian", "hrvatski (Hrvatska)", 1250, 852, "HRV", "HRV" },
    { 0x101A, "hr-BA", "Croatian(Latin, Bosnia and Herzegovina)", "Croatian", "(Latin) hrvatski (Bosna i Hercegovina)", 1250, 852, "BIH", "HRB" },
    { 0x0005, "cs", "Czech", "Czech", "ce�tina", 1250, 852, "CZE", "CSY" },
    { 0x0405, "cs-CZ", "Czech(Czech Republic)", "Czech", "ce�tina (Cesk� republika)", 1250, 852, "CZE", "CSY" },
    { 0x0006, "da", "Danish", "Danish", "dansk", 1252, 850, "DNK", "DAN" },
    { 0x0406, "da-DK", "Danish(Denmark)", "Danish", "dansk (Danmark)", 1252, 850, "DNK", "DAN" },
    { 0x048C, "prs-AF", "Dari(Afghanistan)", "Dari", "??? (?????????)", 1256, 720, "AFG", "PRS" },
    { 0x0065, "div", "Divehi?", "Divehi", "???????????", 0, 1, "MDV", "DIV" },
    { 0x0465, "div-MV", "Divehi(Maldives)?", "Divehi", "?????????? (?????? ??????)?", 0, 1, "MDV", "DIV" },
    { 0x0013, "nl", "Dutch", "Dutch", "Nederlands", 1252, 850, "NLD", "NLD" },
    { 0x0813, "nl-BE", "Dutch(Belgium)", "Dutch", "Nederlands (Belgi�)", 1252, 850, "BEL", "NLB" },
    { 0x0413, "nl-NL", "Dutch(Netherlands)", "Dutch", "Nederlands (Nederland)", 1252, 850, "NLD", "NLD" },
    { 0x0009, "en", "English", "English", "English", 1252, 437, "USA", "ENU" },
    { 0x0C09, "en-AU", "English(Australia)", "English", "English (Australia)", 1252, 850, "AUS", "ENA" },
    { 0x2809, "en-BZ", "English(Belize)", "English", "English (Belize)", 1252, 850, "BLZ", "ENL" },
    { 0x1009, "en-CA", "English(Canada)", "English", "English (Canada)", 1252, 850, "CAN", "ENC" },
    { 0x2409, "en-029", "English(Caribbean)", "English", "English (Caribbean)", 1252, 850, "CAR", "ENB" },
    { 0x4009, "en-IN", "English(India)", "English", "English (India)", 1252, 437, "IND", "ENN" },
    { 0x1809, "en-IE", "English(Ireland)", "English", "English (Eire)", 1252, 850, "IRL", "ENI" },
    { 0x2009, "en-JM", "English(Jamaica)", "English", "English (Jamaica)", 1252, 850, "JAM", "ENJ" },
    { 0x4409, "en-MY", "English(Malaysia)", "English", "English (Malaysia)", 1252, 437, "MYS", "ENM" },
    { 0x1409, "en-NZ", "English(New Zealand)", "English", "English (New Zealand)", 1252, 850, "NZL", "ENZ" },
    { 0x3409, "en-PH", "English(Republic of the Philippines)", "English", "English (Philippines)", 1252, 437, "PHL", "ENP" },
    { 0x4809, "en-SG", "English(Singapore)", "English", "English (Singapore)", 1252, 437, "SGP", "ENE" },
    { 0x1C09, "en-ZA", "English(South Africa)", "English", "English (South Africa)", 1252, 437, "ZAF", "ENS" },
    { 0x2C09, "en-TT", "English(Trinidad and Tobago)", "English", "English (Trinidad y Tobago)", 1252, 850, "TTO", "ENT" },
    { 0x0809, "en-GB", "English(United Kingdom)", "English", "English (United Kingdom)", 1252, 850, "GBR", "ENG" },
    { 0x0409, "en-US", "English(United States)", "English", "English (United States)", 1252, 437, "USA", "ENU" },
    { 0x3009, "en-ZW", "English(Zimbabwe)", "English", "English (Zimbabwe)", 1252, 437, "ZWE", "ENW" },
    { 0x0025, "et", "Estonian", "Estonian", "eesti", 1257, 775, "EST", "ETI" },
    { 0x0425, "et-EE", "Estonian(Estonia)", "Estonian", "eesti (Eesti)", 1257, 775, "EST", "ETI" },
    { 0x0038, "fo", "Faroese", "Faroese", "f�royskt", 1252, 850, "FRO", "FOS" },
    { 0x0438, "fo-FO", "Faroese(Faroe Islands)", "Faroese", "f�royskt (F�royar)", 1252, 850, "FRO", "FOS" },
    { 0x0464, "fil-PH", "Filipino(Philippines)", "Filipino", "Filipino (Pilipinas)", 1252, 437, "PHL", "FPO" },
    { 0x000B, "fi", "Finnish", "Finnish", "suomi", 1252, 850, "FIN", "FIN" },
    { 0x040B, "fi-FI", "Finnish(Finland)", "Finnish", "suomi (Suomi)", 1252, 850, "FIN", "FIN" },
    { 0x000C, "fr", "French", "French", "fran�ais", 1252, 850, "FRA", "FRA" },
    { 0x080C, "fr-BE", "French(Belgium)", "French", "fran�ais (Belgique)", 1252, 850, "BEL", "FRB" },
    { 0x0C0C, "fr-CA", "French(Canada)", "French", "fran�ais (Canada)", 1252, 850, "CAN", "FRC" },
    { 0x040C, "fr-FR", "French(France)", "French", "fran�ais (France)", 1252, 850, "FRA", "FRA" },
    { 0x140C, "fr-LU", "French(Luxembourg)", "French", "fran�ais (Luxembourg)", 1252, 850, "LUX", "FRL" },
    { 0x180C, "fr-MC", "French(Principality of Monaco)", "French", "fran�ais (Principaut� de Monaco)", 1252, 850, "MCO", "FRM" },
    { 0x100C, "fr-CH", "French(Switzerland)", "French", "fran�ais (Suisse)", 1252, 850, "CHE", "FRS" },
    { 0x0462, "fy-NL", "Frisian(Netherlands)", "Frisian", "Frysk (Nederl�n)", 1252, 850, "NLD", "FYN" },
    { 0x0056, "gl", "Galician", "Galician", "galego", 1252, 850, "ESP", "GLC" },
    { 0x0456, "gl-ES", "Galician(Galician)", "Galician", "galego (galego)", 1252, 850, "ESP", "GLC" },
    { 0x0037, "ka", "Georgian", "Georgian", "???????", 0, 1, "GEO", "KAT" },
    { 0x0437, "ka-GE", "Georgian(Georgia)", "Georgian", "??????? (??????????)", 0, 1, "GEO", "KAT" },
    { 0x0007, "de", "German", "German", "Deutsch", 1252, 850, "DEU", "DEU" },
    { 0x0C07, "de-AT", "German(Austria)", "German", "Deutsch (�sterreich)", 1252, 850, "AUT", "DEA" },
    { 0x0407, "de-DE", "German(Germany)", "German", "Deutsch (Deutschland)", 1252, 850, "DEU", "DEU" },
    { 0x1407, "de-LI", "German(Liechtenstein)", "German", "Deutsch (Liechtenstein)", 1252, 850, "LIE", "DEC" },
    { 0x1007, "de-LU", "German(Luxembourg)", "German", "Deutsch (Luxemburg)", 1252, 850, "LUX", "DEL" },
    { 0x0807, "de-CH", "German(Switzerland)", "German", "Deutsch (Schweiz)", 1252, 850, "CHE", "DES" },
    { 0x0008, "el", "Greek", "Greek", "e???????", 1253, 737, "GRC", "ELL" },
    { 0x0408, "el-GR", "Greek(Greece)", "Greek", "e??????? (????da)", 1253, 737, "GRC", "ELL" },
    { 0x046F, "kl-GL", "Greenlandic(Greenland)", "Greenlandic", "kalaallisut (Kalaallit Nunaat)", 1252, 850, "GRL", "KAL" },
    { 0x0047, "gu", "Gujarati", "Gujarati", "???????", 0, 1, "IND", "GUJ" },
    { 0x0447, "gu-IN", "Gujarati(India)", "Gujarati", "??????? (????)", 0, 1, "IND", "GUJ" },
    { 0x0468, "ha-Latn-NG", "Hausa(Latin, Nigeria)", "Hausa", "(Latin) Hausa (Nigeria)", 1252, 437, "NGA", "HAU" },
    { 0x000D, "he", "Hebrew?", "Hebrew", "??????", 1255, 862, "ISR", "HEB" },
    { 0x040D, "he-IL", "Hebrew(Israel)?", "Hebrew", "????? (?????)?", 1255, 862, "ISR", "HEB" },
    { 0x0039, "hi", "Hindi", "Hindi", "?????", 0, 1, "IND", "HIN" },
    { 0x0439, "hi-IN", "Hindi(India)", "Hindi", "????? (????)", 0, 1, "IND", "HIN" },
    { 0x000E, "hu", "Hungarian", "Hungarian", "magyar", 1250, 852, "HUN", "HUN" },
    { 0x040E, "hu-HU", "Hungarian(Hungary)", "Hungarian", "magyar (Magyarorsz�g)", 1250, 852, "HUN", "HUN" },
    { 0x000F, "is", "Icelandic", "Icelandic", "�slenska", 1252, 850, "ISL", "ISL" },
    { 0x040F, "is-IS", "Icelandic(Iceland)", "Icelandic", "�slenska (�sland)", 1252, 850, "ISL", "ISL" },
    { 0x0470, "ig-NG", "Igbo(Nigeria)", "Igbo", "Igbo (Nigeria)", 1252, 437, "NGA", "IBO" },
    { 0x0021, "id", "Indonesian", "Indonesian", "Bahasa Indonesia", 1252, 850, "IDN", "IND" },
    { 0x0421, "id-ID", "Indonesian(Indonesia)", "Indonesian", "Bahasa Indonesia (Indonesia)", 1252, 850, "IDN", "IND" },
    { 0x085D, "iu-Latn-CA", "Inuktitut(Latin, Canada)", "Inuktitut", "(Latin) Inuktitut (Kanatami) (kanata)", 1252, 437, "CAN", "IUK" },
    { 0x045D, "iu-Cans-CA", "Inuktitut(Syllabics, Canada)", "Inuktitut", "?????? (???)", 0, 1, "CAN", "IUS" },
    { 0x083C, "ga-IE", "Irish(Ireland)", "Irish", "Gaeilge (�ire)", 1252, 850, "IRL", "IRE" },
    { 0x0434, "xh-ZA", "isiXhosa(South Africa)", "isiXhosa", "isiXhosa (uMzantsi Afrika)", 1252, 850, "ZAF", "XHO" },
    { 0x0435, "zu-ZA", "isiZulu(South Africa)", "isiZulu", "isiZulu (iNingizimu Afrika)", 1252, 850, "ZAF", "ZUL" },
    { 0x0010, "it", "Italian", "Italian", "italiano", 1252, 850, "ITA", "ITA" },
    { 0x0410, "it-IT", "Italian(Italy)", "Italian", "italiano (Italia)", 1252, 850, "ITA", "ITA" },
    { 0x0810, "it-CH", "Italian(Switzerland)", "Italian", "italiano (Svizzera)", 1252, 850, "CHE", "ITS" },
    { 0x0011, "ja", "Japanese", "Japanese", "???", 932, 932, "JPN", "JPN" },
    { 0x0411, "ja-JP", "Japanese(Japan)", "Japanese", "??? (??)", 932, 932, "JPN", "JPN" },
    { 0x004B, "kn", "Kannada", "Kannada", "?????", 0, 1, "IND", "KDI" },
    { 0x044B, "kn-IN", "Kannada(India)", "Kannada", "????? (????)", 0, 1, "IND", "KDI" },
    { 0x003F, "kk", "Kazakh", "Kazakh", "?????b", 1251, 866, "KAZ", "KKZ" },
    { 0x043F, "kk-KZ", "Kazakh(Kazakhstan)", "Kazakh", "????? (?????????)", 1251, 866, "KAZ", "KKZ" },
    { 0x0453, "km-KH", "Khmer(Cambodia)", "Khmer", "????? (???????)", 0, 1, "KHM", "KHM" },
    { 0x0486, "qut-GT", "K'iche(Guatemala)", "K'iche", "K'iche (Guatemala)", 1252, 850, "GTM", "QUT" },
    { 0x0487, "rw-RW", "Kinyarwanda(Rwanda)", "Kinyarwanda", "Kinyarwanda (Rwanda)", 1252, 437, "RWA", "KIN" },
    { 0x0041, "sw", "Kiswahili", "Kiswahili", "Kiswahili", 1252, 437, "KEN", "SWK" },
    { 0x0441, "sw-KE", "Kiswahili(Kenya)", "Kiswahili", "Kiswahili (Kenya)", 1252, 437, "KEN", "SWK" },
    { 0x0057, "kok", "Konkani", "Konkani", "??????", 0, 1, "IND", "KNK" },
    { 0x0457, "kok-IN", "Konkani(India)", "Konkani", "?????? (????)", 0, 1, "IND", "KNK" },
    { 0x0012, "ko", "Korean", "Korean", "???", 949, 949, "KOR", "KOR" },
    { 0x0412, "ko-KR", "Korean(Korea)", "Korean", "??? (????)", 949, 949, "KOR", "KOR" },
    { 0x0040, "ky", "Kyrgyz", "Kyrgyz", "??????", 1251, 866, "KGZ", "KYR" },
    { 0x0440, "ky-KG", "Kyrgyz(Kyrgyzstan)", "Kyrgyz", "?????? (??????????)", 1251, 866, "KGZ", "KYR" },
    { 0x0454, "lo-LA", "Lao(Lao P.D.R.)", "Lao", "??? (?.?.?. ???)", 0, 1, "LAO", "LAO" },
    { 0x0026, "lv", "Latvian", "Latvian", "latvie�u", 1257, 775, "LVA", "LVI" },
    { 0x0426, "lv-LV", "Latvian(Latvia)", "Latvian", "latvie�u (Latvija)", 1257, 775, "LVA", "LVI" },
    { 0x0027, "lt", "Lithuanian", "Lithuanian", "lietuviu", 1257, 775, "LTU", "LTH" },
    { 0x0427, "lt-LT", "Lithuanian(Lithuania)", "Lithuanian", "lietuviu (Lietuva)", 1257, 775, "LTU", "LTH" },
    { 0x082E, "wee-DE", "Lower", "Sorbian", "(Germany) Lower Sorbian dolnoserb�cina (Nimska)", 1252, 850, "GER", "DSB" },
    { 0x046E, "lb-LU", "Luxembourgish(Luxembourg)", "Luxembourgish", "L�tzebuergesch (Luxembourg)", 1252, 850, "LUX", "LBX" },
    { 0x002F, "mk", "Macedonian", "Macedonian", "(FYROM) ?????????? ?????", 1251, 866, "MKD", "MKI" },
    { 0x042F, "mk-MK", "Macedonian(Former Yugoslav Republic of Macedonia)", "Macedonian", "(FYROM) ?????????? ????? (??????????)", 1251, 866, "MKD", "MKI" },
    { 0x003E, "ms", "Malay", "Malay", "Bahasa Malaysia", 1252, 850, "MYS", "MSL" },
    { 0x083E, "ms-BN", "Malay(Brunei Darussalam)", "Malay", "Bahasa Malaysia (Brunei Darussalam)", 1252, 850, "BRN", "MSB" },
    { 0x043E, "ms-MY", "Malay(Malaysia)", "Malay", "Bahasa Malaysia (Malaysia)", 1252, 850, "MYS", "MSL" },
    { 0x044C, "ml-IN", "Malayalam(India)", "Malayalam", "?????? (?????)", 0, 1, "IND", "MYM" },
    { 0x043A, "mt-MT", "Maltese(Malta)", "Maltese", "Malti (Malta)", 0, 1, "MLT", "MLT" },
    { 0x0481, "mi-NZ", "Maori(New Zealand)", "Maori", "Reo Maori (Aotearoa)", 0, 1, "NZL", "MRI" },
    { 0x047A, "arn-CL", "Mapudungun(Chile)", "Mapudungun", "Mapudungun (Chile)", 1252, 850, "CHL", "MPD" },
    { 0x004E, "mr", "Marathi", "Marathi", "?????", 0, 1, "IND", "MAR" },
    { 0x044E, "mr-IN", "Marathi(India)", "Marathi", "????? (????)", 0, 1, "IND", "MAR" },
    { 0x047C, "moh-CA", "Mohawk(Mohawk)", "Mohawk", "Kanien'k�ha (Canada)", 1252, 850, "CAN", "MWK" },
    { 0x0050, "mn", "Mongolian", "Mongolian", "(Cyrillic) ?????? ???", 1251, 866, "MNG", "MON" },
    { 0x0450, "mn-MN", "Mongolian(Cyrillic, Mongolia)", "Mongolian", "(Cyrillic) ?????? ??? (?????? ???)", 1251, 866, "MNG", "MON" },
    { 0x0850, "mn-Mong-CN", "Mongolian(Traditional Mongolian, PRC)", "Mongolian", "(Traditional Mongolian) ?????? ???? (?????? ?????????? ??????? ???? ????)", 0, 1, "CHN", "MNG" },
    { 0x0461, "ne-NP", "Nepali(Nepal)", "Nepali", "?????? (?????)", 0, 1, "NEP", "NEP" },
    { 0x0014, "no", "Norwegian", "Norwegian", "(Bokm�l) norsk", 1252, 850, "NOR", "NOR" },
    { 0x0414, "nb-NO", "Norwegian,", "Bokm�l", "(Norway) Norwegian (Bokm�l) norsk, bokm�l (Norge)", 1252, 850, "NOR", "NOR" },
    { 0x0814, "nn-NO", "Norwegian,", "Nynorsk", "(Norway) Norwegian (Nynorsk) norsk, nynorsk (Noreg)", 1252, 850, "NOR", "NON" },
    { 0x0482, "oc-FR", "Occitan(France)", "Occitan", "Occitan (Fran�a)", 1252, 850, "FRA", "OCI" },
    { 0x0448, "or-IN", "Oriya(India)", "Oriya", "???? (????)", 0, 1, "IND", "ORI" },
    { 0x0463, "ps-AF", "Pashto(Afghanistan)", "Pashto", "???? (?????????)", 0, 1, "AFG", "PAS" },
    { 0x0029, "fa", "Persian?", "Persian", "??????", 1256, 720, "IRN", "FAR" },
    { 0x0429, "fa-IR", "Persian?", "Persian", "????? (?????)?", 1256, 720, "IRN", "FAR" },
    { 0x0015, "pl", "Polish", "Polish", "polski", 1250, 852, "POL", "PLK" },
    { 0x0415, "pl-PL", "Polish(Poland)", "Polish", "polski (Polska)", 1250, 852, "POL", "PLK" },
    { 0x0016, "pt", "Portuguese", "Portuguese", "Portugu�s", 1252, 850, "BRA", "PTB" },
    { 0x0416, "pt-BR", "Portuguese(Brazil)", "Portuguese", "Portugu�s (Brasil)", 1252, 850, "BRA", "PTB" },
    { 0x0816, "pt-PT", "Portuguese(Portugal)", "Portuguese", "portugu�s (Portugal)", 1252, 850, "PRT", "PTG" },
    { 0x0046, "pa", "Punjabi", "Punjabi", "??????", 0, 1, "IND", "PAN" },
    { 0x0446, "pa-IN", "Punjabi(India)", "Punjabi", "?????? (????)", 0, 1, "IND", "PAN" },
    { 0x046B, "quz-BO", "Quechua(Bolivia)", "Quechua", "runasimi (Bolivia Suyu)", 1252, 850, "BOL", "QUB" },
    { 0x086B, "quz-EC", "Quechua(Ecuador)", "Quechua", "runasimi (Ecuador Suyu)", 1252, 850, "ECU", "QUE" },
    { 0x0C6B, "quz-PE", "Quechua(Peru)", "Quechua", "runasimi (Peru Suyu)", 1252, 850, "PER", "QUP" },
    { 0x0018, "ro", "Romanian", "Romanian", "rom�na", 1250, 852, "ROM", "ROM" },
    { 0x0418, "ro-RO", "Romanian(Romania)", "Romanian", "rom�na (Rom�nia)", 1250, 852, "ROM", "ROM" },
    { 0x0417, "rm-CH", "Romansh(Switzerland)", "Romansh", "Rumantsch (Svizra)", 1252, 850, "CHE", "RMC" },
    { 0x0019, "ru", "Russian", "Russian", "???????", 1251, 866, "RUS", "RUS" },
    { 0x0419, "ru-RU", "Russian(Russia)", "Russian", "??????? (??????)", 1251, 866, "RUS", "RUS" },
    { 0x243B, "smn-FI", "Sami,", "Inari", "(Finland) Sami (Inari) s�mikiel� (Suom�)", 1252, 850, "FIN", "SMN" },
    { 0x103B, "smj-NO", "Sami,", "Lule", "(Norway) Sami (Lule) julevus�megiella (Vuodna)", 1252, 850, "NOR", "SMJ" },
    { 0x143B, "smj-SE", "Sami,", "Lule", "(Sweden) Sami (Lule) julevus�megiella (Svierik)", 1252, 850, "SWE", "SMK" },
    { 0x0C3B, "se-FI", "Sami,", "Northern", "(Finland) Sami (Northern) davvis�megiella (Suopma)", 1252, 850, "FIN", "SMG" },
    { 0x043B, "se-NO", "Sami,", "Northern", "(Norway) Sami (Northern) davvis�megiella (Norga)", 1252, 850, "NOR", "SME" },
    { 0x083B, "se-SE", "Sami,", "Northern", "(Sweden) Sami (Northern) davvis�megiella (Ruotta)", 1252, 850, "SWE", "SMF" },
    { 0x203B, "sms-FI", "Sami,", "Skolt", "(Finland) Sami (Skolt) s��m�ki�ll (L��ddj�nnam)", 1252, 850, "FIN", "SMS" },
    { 0x183B, "sma-NO", "Sami,", "Southern", "(Norway) Sami (Southern) �arjelsaemiengiele (N��rje)", 1252, 850, "NOR", "SMA" },
    { 0x1C3B, "sma-SE", "Sami,", "Southern", "(Sweden) Sami (Southern) �arjelsaemiengiele (Sveerje)", 1252, 850, "SWE", "SMB" },
    { 0x004F, "sa", "Sanskrit", "Sanskrit", "???????", 0, 1, "IND", "SAN" },
    { 0x044F, "sa-IN", "Sanskrit(India)", "Sanskrit", "??????? (??????)", 0, 1, "IND", "SAN" },
    { 0x7C1A, "sr", "Serbian", "Serbian", "(Latin) srpski", 1251, 855, "SCG", "SRB" },
    { 0x1C1A, "sr-Cyrl-BA", "Serbian(Cyrillic, Bosnia and Herzegovina)", "Serbian", "(Cyrillic) ?????? (????? ? ???????????)", 1251, 855, "BIH", "SRN" },
    { 0x0C1A, "sr-Cyrl-SP", "Serbian(Cyrillic, Serbia)", "Serbian", "(Cyrillic) ?????? (?????? ? ???? ????)", 1251, 855, "SCG", "SRB" },
    { 0x181A, "sr-Latn-BA", "Serbian(Latin, Bosnia and Herzegovina)", "Serbian", "(Latin) srpski (Bosna i Hercegovina)", 1250, 852, "BIH", "SRS" },
    { 0x081A, "sr-Latn-SP", "Serbian(Latin, Serbia)", "Serbian", "(Latin) srpski (Srbija i Crna Gora)", 1250, 852, "SCG", "SRL" },
    { 0x046C, "nso-ZA", "Sesotho", "sa", "Leboa (South Africa) Sesotho sa Leboa Sesotho sa Leboa (Afrika Borwa)", 1252, 850, "ZAF", "NSO" },
    { 0x0432, "tn-ZA", "Setswana(South Africa)", "Setswana", "Setswana (Aforika Borwa)", 1252, 850, "ZAF", "TSN" },
    { 0x045B, "si-LK", "Sinhala(Sri Lanka)", "Sinhala", "???? (????? ????)", 0, 1, "LKA", "SIN" },
    { 0x001B, "sk", "Slovak", "Slovak", "slovencina", 1250, 852, "SVK", "SKY" },
    { 0x041B, "sk-SK", "Slovak(Slovakia)", "Slovak", "slovencina (Slovensk� republika)", 1250, 852, "SVK", "SKY" },
    { 0x0024, "sl", "Slovenian", "Slovenian", "slovenski", 1250, 852, "SVN", "SLV" },
    { 0x0424, "sl-SI", "Slovenian(Slovenia)", "Slovenian", "slovenski (Slovenija)", 1250, 852, "SVN", "SLV" },
    { 0x000A, "es", "Spanish", "Spanish", "espa�ol", 1252, 850, "ESP", "ESP" },
    { 0x2C0A, "es-AR", "Spanish(Argentina)", "Spanish", "Espa�ol (Argentina)", 1252, 850, "ARG", "ESS" },
    { 0x400A, "es-BO", "Spanish(Bolivia)", "Spanish", "Espa�ol (Bolivia)", 1252, 850, "BOL", "ESB" },
    { 0x340A, "es-CL", "Spanish(Chile)", "Spanish", "Espa�ol (Chile)", 1252, 850, "CHL", "ESL" },
    { 0x240A, "es-CO", "Spanish(Colombia)", "Spanish", "Espa�ol (Colombia)", 1252, 850, "COL", "ESO" },
    { 0x140A, "es-CR", "Spanish(Costa Rica)", "Spanish", "Espa�ol (Costa Rica)", 1252, 850, "CRI", "ESC" },
    { 0x1C0A, "es-DO", "Spanish(Dominican Republic)", "Spanish", "Espa�ol (Rep�blica Dominicana)", 1252, 850, "DOM", "ESD" },
    { 0x300A, "es-EC", "Spanish(Ecuador)", "Spanish", "Espa�ol (Ecuador)", 1252, 850, "ECU", "ESF" },
    { 0x440A, "es-SV", "Spanish(El Salvador)", "Spanish", "Espa�ol (El Salvador)", 1252, 850, "SLV", "ESE" },
    { 0x100A, "es-GT", "Spanish(Guatemala)", "Spanish", "Espa�ol (Guatemala)", 1252, 850, "GTM", "ESG" },
    { 0x480A, "es-HN", "Spanish(Honduras)", "Spanish", "Espa�ol (Honduras)", 1252, 850, "HND", "ESH" },
    { 0x080A, "es-MX", "Spanish(Mexico)", "Spanish", "Espa�ol (M�xico)", 1252, 850, "MEX", "ESM" },
    { 0x4C0A, "es-NI", "Spanish(Nicaragua)", "Spanish", "Espa�ol (Nicaragua)", 1252, 850, "NIC", "ESI" },
    { 0x180A, "es-PA", "Spanish(Panama)", "Spanish", "Espa�ol (Panam�)", 1252, 850, "PAN", "ESA" },
    { 0x3C0A, "es-PY", "Spanish(Paraguay)", "Spanish", "Espa�ol (Paraguay)", 1252, 850, "PRY", "ESZ" },
    { 0x280A, "es-PE", "Spanish(Peru)", "Spanish", "Espa�ol (Per�)", 1252, 850, "PER", "ESR" },
    { 0x500A, "es-PR", "Spanish(Puerto Rico)", "Spanish", "Espa�ol (Puerto Rico)", 1252, 850, "PRI", "ESU" },
    { 0x0C0A, "es-ES", "Spanish(Spain)", "Spanish", "espa�ol (Espa�a)", 1252, 850, "ESP", "ESN" },
    { 0x540A, "es-US", "Spanish(United States)", "Spanish", "Espa�ol (Estados Unidos)", 1252, 850, "USA", "EST" },
    { 0x380A, "es-UY", "Spanish(Uruguay)", "Spanish", "Espa�ol (Uruguay)", 1252, 850, "URY", "ESY" },
    { 0x200A, "es-VE", "Spanish(Venezuela)", "Spanish", "Espa�ol (Republica Bolivariana de Venezuela)", 1252, 850, "VEN", "ESV" },
    { 0x001D, "sv", "Swedish", "Swedish", "svenska", 1252, 850, "SWE", "SVE" },
    { 0x081D, "sv-FI", "Swedish(Finland)", "Swedish", "svenska (Finland)", 1252, 850, "FIN", "SVF" },
    { 0x041D, "sv-SE", "Swedish(Sweden)", "Swedish", "svenska (Sverige)", 1252, 850, "SWE", "SVE" },
    { 0x005A, "syr", "Syriac?", "Syriac", "???????", 0, 1, "SYR", "SYR" },
    { 0x045A, "syr-SY", "Syriac(Syria)?", "Syriac", "?????? (?????)?", 0, 1, "SYR", "SYR" },
    { 0x0428, "tg-Cyrl-TJ", "Tajik(Cyrillic, Tajikistan)", "Tajik", "(Cyrillic) ?????? (??????????)", 1251, 866, "TAJ", "TAJ" },
    { 0x085F, "tzm-Latn-DZ", "Tamazight(Latin, Algeria)", "Tamazight", "(Latin) Tamazight (Djaza�r)", 1252, 850, "DZA", "TZM" },
    { 0x0049, "ta", "Tamil", "Tamil", "?????", 0, 1, "IND", "TAM" },
    { 0x0449, "ta-IN", "Tamil(India)", "Tamil", "????? (???????)", 0, 1, "IND", "TAM" },
    { 0x0044, "tt", "Tatar", "Tatar", "?????", 1251, 866, "RUS", "TTT" },
    { 0x0444, "tt-RU", "Tatar(Russia)", "Tatar", "????? (??????)", 1251, 866, "RUS", "TTT" },
    { 0x004A, "te", "Telugu", "Telugu", "??????", 0, 1, "IND", "TEL" },
    { 0x044A, "te-IN", "Telugu(India)", "Telugu", "?????? (???? ????)", 0, 1, "IND", "TEL" },
    { 0x001E, "th", "Thai", "Thai", "???", 874, 874, "THA", "THA" },
    { 0x041E, "th-TH", "Thai(Thailand)", "Thai", "??? (???)", 874, 874, "THA", "THA" },
    { 0x0451, "bo-CN", "Tibetan(PRC)", "Tibetan", "??????? (??????????????????????????????????)", 0, 1, "CHN", "BOB" },
    { 0x001F, "tr", "Turkish", "Turkish", "T�rk�e", 1254, 857, "TUR", "TRK" },
    { 0x041F, "tr-TR", "Turkish(Turkey)", "Turkish", "T�rk�e (T�rkiye)", 1254, 857, "TUR", "TRK" },
    { 0x0442, "tk-TM", "Turkmen(Turkmenistan)", "Turkmen", "t�rkmen�e (T�rkmenistan)", 1250, 852, "TKM", "TUK" },
    { 0x0480, "ug-CN", "Uighur(PRC)", "Uighur", "?????? ?????? (?????? ???? ???????????)", 1256, 720, "CHN", "UIG" },
    { 0x0022, "uk", "Ukrainian", "Ukrainian", "??????????", 1251, 866, "UKR", "UKR" },
    { 0x0422, "uk-UA", "Ukrainian(Ukraine)", "Ukrainian", "?????????? (???????)", 1251, 866, "UKR", "UKR" },
    { 0x042E, "wen-DE", "Upper", "Sorbian", "(Germany) Upper Sorbian hornjoserb�cina (Nemska)", 1252, 850, "GER", "HSB" },
    { 0x0020, "ur", "Urdu?", "Urdu", "??????", 1256, 720, "PAK", "URD" },
    { 0x0420, "ur-PK", "Urdu(Islamic Republic of Pakistan)?", "Urdu", "????? (???????)?", 1256, 720, "PAK", "URD" },
    { 0x0043, "uz", "Uzbek", "Uzbek", "(Latin) U'zbek", 1254, 857, "UZB", "UZB" },
    { 0x0843, "uz-Cyrl-UZ", "Uzbek(Cyrillic, Uzbekistan)", "Uzbek", "(Cyrillic) ????? (??????????)", 1251, 866, "UZB", "UZB" },
    { 0x0443, "uz-Latn-UZ", "Uzbek(Latin, Uzbekistan)", "Uzbek", "(Latin) U'zbek (U'zbekiston Respublikasi)", 1254, 857, "UZB", "UZB" },
    { 0x002A, "vi", "Vietnamese", "Vietnamese", "Ti�ng Vi�?t", 1258, 1258, "VNM", "VIT" },
    { 0x042A, "vi-VN", "Vietnamese(Vietnam)", "Vietnamese", "Ti�ng Vi�?t (Vi�?t Nam)", 1258, 1258, "VNM", "VIT" },
    { 0x0452, "cy-GB", "Welsh(United Kingdom)", "Welsh", "Cymraeg (y Deyrnas Unedig)", 1252, 850, "GBR", "CYM" },
    { 0x0488, "wo-SN", "Wolof(Senegal)", "Wolof", "Wolof (S�n�gal)", 1252, 850, "SEN", "WOL" },
    { 0x0485, "sah-RU", "Yakut(Russia)", "Yakut", "???? (??????)", 1251, 866, "RUS", "SAH" },
    { 0x0478, "ii-CN", "Yi(PRC)", "Yi", "???? (???????)", 0, 1, "CHN", "III" },
    { 0x046A, "yo-NG", "Yoruba(Nigeria)", "Yoruba", "Yoruba (Nigeria)", 1252, 437, "NGA", "YOR" },

    // LAST ENTRY
    { 0,       0,    0,           0,           0,          0,    0,    0,    0     }
};

const char *get_name_from_clid( int clid )
{
    PLCID2NAMES p2n = &LCID2Names[0];
    while ( p2n->nm ) {
        if (clid == p2n->LCID)
            return p2n->nm;
        p2n++;
    }
    return NULL;
}

/* ==========================================

typedef struct threadlocaleinfostruct {
        int refcount;
        unsigned int lc_codepage;
        unsigned int lc_collate_cp;
        unsigned long lc_handle[6]; / * LCID * /
        LC_ID lc_id[6];
        struct {
            char *locale;
            wchar_t *wlocale;
            int *refcount;
            int *wrefcount;
        } lc_category[6];
        int lc_clike;
        int mb_cur_max;
        int * lconv_intl_refcount;
        int * lconv_num_refcount;
        int * lconv_mon_refcount;
        struct lconv * lconv;
        int * ctype1_refcount;
        unsigned short * ctype1;
        const unsigned short * pctype;
        const unsigned char * pclmap;
        const unsigned char * pcumap;
        struct __lc_time_data * lc_time_curr;
} threadlocinfo;

   ===================================================== */
char *nl_langinfo(nl_item item)
{
    char *l, *p;
    _locale_t lt; 
  
    if (item != DEF_CODESET)
        return NULL;
    //p = local_charset();
    p = setlocale(LC_ALL, "");
    if (p) {
        printf("setlocal: %s\n", p);
    }
    lt = _get_current_locale();
    if (lt) {
        //const char *ccp = get_name_from_clid( lt->locinfo );
        _free_locale(lt);
    }
    if (((l = getenv("LC_ALL"))   && *l) ||
        ((l = getenv("LC_CTYPE")) && *l) ||
        ((l = getenv("LANG"))     && *l)) 
    {
        /* check standardized locales */
        if (!strcmp(l, "C") || !strcmp(l, "POSIX"))
            return C_CODESET;

        /* check for encoding name fragment */
        if (strstr(l, "UTF") || strstr(l, "utf"))
            return "UTF-8";

        if ((p = strstr(l, "8859-"))) 
        {
            memcpy(buf, "ISO-8859-\0\0", 12);
            p += 5;
            if (digit(*p)) 
            {
                buf[9] = *p++;
                if (digit(*p)) 
                    buf[10] = *p++;
                return buf;
            }
        }

        if (strstr(l, "KOI8-R")) 
            return "KOI8-R";

        if (strstr(l, "KOI8-U"))
            return "KOI8-U";

        if (strstr(l, "620")) 
            return "TIS-620";

        if (strstr(l, "2312")) 
            return "GB2312";

        if (strstr(l, "HKSCS")) 
            return "Big5HKSCS";   /* no MIME charset */

        if (strstr(l, "Big5") || strstr(l, "BIG5"))
            return "Big5";

        if (strstr(l, "GBK"))
            return "GBK";           /* no MIME charset */

        if (strstr(l, "18030"))
            return "GB18030";     /* no MIME charset */

        if (strstr(l, "Shift_JIS") || strstr(l, "SJIS"))
            return "Shift_JIS";

        /* check for conclusive modifier */
        if (strstr(l, "euro")) 
            return "ISO-8859-15";

        /* check for language (and perhaps country) codes */
        if (strstr(l, "zh_TW")) 
            return "Big5";

        if (strstr(l, "zh_HK"))
            return "Big5HKSCS";   /* no MIME charset */

        if (strstr(l, "zh"))
            return "GB2312";

        if (strstr(l, "ja"))
            return "EUC-JP";

        if (strstr(l, "ko"))
            return "EUC-KR";

        if (strstr(l, "ru"))
            return "KOI8-R";

        if (strstr(l, "uk"))
            return "KOI8-U";

        if (strstr(l, "pl") || strstr(l, "hr") ||
	        strstr(l, "hu") || strstr(l, "cs") ||
            strstr(l, "sk") || strstr(l, "sl"))
            return "ISO-8859-2";

        if (strstr(l, "eo") || strstr(l, "mt"))
            return "ISO-8859-3";

        if (strstr(l, "el"))
            return "ISO-8859-7";

        if (strstr(l, "he"))
            return "ISO-8859-8";

        if (strstr(l, "tr"))
            return "ISO-8859-9";

        if (strstr(l, "th"))
            return "TIS-620";      /* or ISO-8859-11 */

        if (strstr(l, "lt"))
            return "ISO-8859-13";

        if (strstr(l, "cy"))
            return "ISO-8859-14";

        if (strstr(l, "ro"))
            return "ISO-8859-2";   /* or ISO-8859-16 */

        if (strstr(l, "am") || strstr(l, "vi"))
            return "UTF-8";

        /* Send me further rules if you like, but don't forget that we are
         * *only* interested in locale naming conventions on platforms
         * that do not already provide an nl_langinfo(CODESET) implementation. */
        //return "ISO-8859-1"; /* should perhaps be "UTF-8" instead */
        //return "UTF-8"; /* have set default return as "UTF-8"!!! */
    }
    return C_CODESET;
}

/* For a demo, compile with "gcc -W -Wall -o langinfo -D TEST langinfo.c" */

int test_codeset( int CS )
{
    char *cs = nl_langinfo(CS);
	printf("nl_langinfo: returned '%s'\n", cs ? cs : "<null>");
	return 0;
}

// eof

