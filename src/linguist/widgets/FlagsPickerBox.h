#ifndef FLAGSPICKERBOX_H
#define FLAGSPICKERBOX_H
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QIcon>
#include <QStringList>
#include <QSize>

class ComboBoxDelegate : public QStyledItemDelegate {
public:
    ComboBoxDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        painter->save();
        painter->setClipRect(opt.rect);

        // Espacement personnalisé
        int space = 9;

        // Dessiner l'icône
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        QSize iconSize = icon.actualSize(opt.rect.size());
        icon.paint(painter, opt.rect.x(), opt.rect.y(), iconSize.width(), iconSize.height(), Qt::AlignLeft);

        // Dessiner le texte
        QRect textRect = opt.rect.adjusted(iconSize.width() + space, 0, 0, 0);
        QFont font = painter->font();
        font.setPointSize(12);
        painter->setFont(font);
        painter->drawText(textRect, opt.displayAlignment, index.data(Qt::DisplayRole).toString());

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setWidth(size.width() + 20);
        return size;
    }
};

class FlagsPickerBox : public QComboBox {
    Q_OBJECT
public:
    explicit FlagsPickerBox(QWidget *parent = nullptr) : QComboBox(parent) {
        setItemDelegate(new ComboBoxDelegate(this)); 
        loadFlags();
    }


    // Méthode statique pour récupérer l'icône du drapeau selon le code de langue
    static QIcon getFlagIcon(const QString &language) {
        QString prefix = ":/4x3/ressources/assets/flags/";
        QString imagePath;
        QString contryCode = languageToFlagMap.value(language);
        if(contryCode == "") contryCode = languageToFlagMap.value(m_languageMap.value(language));
        imagePath = prefix + contryCode + ".svg";

        QIcon flagIcon(imagePath);
        if (flagIcon.isNull()) {
            qDebug() << "Flag icon not found for language:" << language << imagePath;
        }

        return flagIcon;
    }

    static QString codeLanguageFromLanguage(const QString &language) {
        return m_languageMap.value(language);
    }

    static QString languageFromCodeLanguage(const QString &codeLanguage) {
        return m_codeToLanguageMap.value(codeLanguage);
    }

    static QString codeFlagFromCodeLanguage(const QString &code) {
        return languageToFlagMap.value(code);
    }

    static QStringList availableLanguages()
    {
        return m_languageMap.keys();
    }


private:
    void loadFlags() {
        // Assurez-vous que le préfixe du chemin d'accès aux drapeaux est correct
        QString prefix = ":/4x3/ressources/assets/flags/";

        // Parcourir la map m_languageMap pour ajouter chaque langue
        for (auto it = m_languageMap.cbegin(); it != m_languageMap.cend(); ++it) {
            QString languageName = it.key();
            QString languageCode = it.value();

            // Créer le chemin d'accès pour le fichier SVG du drapeau
            QString imagePath = prefix + codeFlagFromCodeLanguage(languageCode) + ".svg";

            // Charger le SVG en tant qu'icône
            QIcon flagIcon(imagePath);

            if (flagIcon.isNull()) {
                qDebug() << "Flag icon not found for language code:" << languageCode;
                continue;
            }

            // Ajouter l'élément dans le QComboBox avec l'icône et le nom de la langue
            addItem(flagIcon, languageName);
            // Stocker le code de langue en tant que data dans l'élément
            setItemData(count() - 1, languageCode, Qt::UserRole);
        }
    }

    static const inline QMap<QString, QString> m_languageMap = {
        { "Afrikaans", "af" },
        { "Albanian", "sq" },
        { "Amharic", "am" },
        { "Arabic", "ar" },
        { "Armenian", "hy" },
        { "Azerbaijani", "az" },
        { "Basque", "eu" },
        { "Belarusian", "be" },
        { "Bengali", "bn" },
        { "Bosnian", "bs" },
        { "Bulgarian", "bg" },
        { "Catalan", "ca" },
        { "Cebuano", "ceb" },
        { "Chinese", "zh" },
        { "Corsican", "co" },
        { "Croatian", "hr" },
        { "Czech", "cs" },
        { "Danish", "da" },
        { "Dutch", "nl" },
        { "English", "en" },
        { "Esperanto", "eo" },
        { "Estonian", "et" },
        { "Filipino", "fil" },
        { "Finnish", "fi" },
        { "French", "fr" },
        { "Frisian", "fy" },
        { "Galician", "gl" },
        { "Georgian", "ka" },
        { "German", "de" },
        { "Greek", "el" },
        { "Gujarati", "gu" },
        { "Haitian Creole", "ht" },
        { "Hausa", "ha" },
        { "Hawaiian", "haw" },
        { "Hebrew", "he" },
        { "Hindi", "hi" },
        { "Hmong", "hmn" },
        { "Hungarian", "hu" },
        { "Icelandic", "is" },
        { "Igbo", "ig" },
        { "Indonesian", "id" },
        { "Irish", "ga" },
        { "Italian", "it" },
        { "Japanese", "ja" },
        { "Javanese", "jv" },
        { "Kannada", "kn" },
        { "Kazakh", "kk" },
        { "Khmer", "km" },
        { "Kinyarwanda", "rw" },
        { "Korean", "ko" },
        { "Kurdish", "ku" },
        { "Kyrgyz", "ky" },
        { "Lao", "lo" },
        { "Latin", "la" },
        { "Latvian", "lv" },
        { "Lithuanian", "lt" },
        { "Luxembourgish", "lb" },
        { "Macedonian", "mk" },
        { "Malagasy", "mg" },
        { "Malay", "ms" },
        { "Malayalam", "ml" },
        { "Maltese", "mt" },
        { "Maori", "mi" },
        { "Marathi", "mr" },
        { "Mongolian", "mn" },
        { "Myanmar (Burmese)", "my" },
        { "Nepali", "ne" },
        { "Norwegian", "no" },
        { "Nyanja (Chichewa)", "ny" },
        { "Odia (Oriya)", "or" },
        { "Pashto", "ps" },
        { "Persian", "fa" },
        { "Polish", "pl" },
        { "Portuguese", "pt" },
        { "Punjabi", "pa" },
        { "Romanian", "ro" },
        { "Russian", "ru" },
        { "Samoan", "sm" },
        { "Scots Gaelic", "gd" },
        { "Serbian", "sr" },
        { "Sesotho", "st" },
        { "Shona", "sn" },
        { "Sindhi", "sd" },
        { "Sinhala", "si" },
        { "Slovak", "sk" },
        { "Slovenian", "sl" },
        { "Somali", "so" },
        { "Spanish", "es" },
        { "Sundanese", "su" },
        { "Swahili", "sw" },
        { "Swedish", "sv" },
        { "Tajik", "tg" },
        { "Tamil", "ta" },
        { "Tatar", "tt" },
        { "Telugu", "te" },
        { "Thai", "th" },
        { "Turkish", "tr" },
        { "Turkmen", "tk" },
        { "Ukrainian", "uk" },
        { "Urdu", "ur" },
        { "Uyghur", "ug" },
        { "Uzbek", "uz" },
        { "Vietnamese", "vi" },
        { "Welsh", "cy" },
        { "Xhosa", "xh" },
        { "Yiddish", "yi" },
        { "Yoruba", "yo" },
        { "Zulu", "zu" }
    };

    static const inline QMap<QString, QString> m_codeToLanguageMap = {
        { "af", "Afrikaans" },
        { "sq", "Albanian" },
        { "am", "Amharic" },
        { "ar", "Arabic" },
        { "hy", "Armenian" },
        { "az", "Azerbaijani" },
        { "eu", "Basque" },
        { "be", "Belarusian" },
        { "bn", "Bengali" },
        { "bs", "Bosnian" },
        { "bg", "Bulgarian" },
        { "ca", "Catalan" },
        { "ceb", "Cebuano" },
        { "zh", "Chinese" },
        { "co", "Corsican" },
        { "hr", "Croatian" },
        { "cs", "Czech" },
        { "da", "Danish" },
        { "nl", "Dutch" },
        { "en", "English" },
        { "eo", "Esperanto" },
        { "et", "Estonian" },
        { "fil", "Filipino" },
        { "fi", "Finnish" },
        { "fr", "French" },
        { "fy", "Frisian" },
        { "gl", "Galician" },
        { "ka", "Georgian" },
        { "de", "German" },
        { "el", "Greek" },
        { "gu", "Gujarati" },
        { "ht", "Haitian Creole" },
        { "ha", "Hausa" },
        { "haw", "Hawaiian" },
        { "he", "Hebrew" },
        { "hi", "Hindi" },
        { "hmn", "Hmong" },
        { "hu", "Hungarian" },
        { "is", "Icelandic" },
        { "ig", "Igbo" },
        { "id", "Indonesian" },
        { "ga", "Irish" },
        { "it", "Italian" },
        { "ja", "Japanese" },
        { "jv", "Javanese" },
        { "kn", "Kannada" },
        { "kk", "Kazakh" },
        { "km", "Khmer" },
        { "rw", "Kinyarwanda" },
        { "ko", "Korean" },
        { "ku", "Kurdish" },
        { "ky", "Kyrgyz" },
        { "lo", "Lao" },
        { "la", "Latin" },
        { "lv", "Latvian" },
        { "lt", "Lithuanian" },
        { "lb", "Luxembourgish" },
        { "mk", "Macedonian" },
        { "mg", "Malagasy" },
        { "ms", "Malay" },
        { "ml", "Malayalam" },
        { "mt", "Maltese" },
        { "mi", "Maori" },
        { "mr", "Marathi" },
        { "mn", "Mongolian" },
        { "my", "Myanmar (Burmese)" },
        { "ne", "Nepali" },
        { "no", "Norwegian" },
        { "ny", "Nyanja (Chichewa)" },
        { "or", "Odia (Oriya)" },
        { "ps", "Pashto" },
        { "fa", "Persian" },
        { "pl", "Polish" },
        { "pt", "Portuguese" },
        { "pa", "Punjabi" },
        { "ro", "Romanian" },
        { "ru", "Russian" },
        { "sm", "Samoan" },
        { "gd", "Scots Gaelic" },
        { "sr", "Serbian" },
        { "st", "Sesotho" },
        { "sn", "Shona" },
        { "sd", "Sindhi" },
        { "si", "Sinhala" },
        { "sk", "Slovak" },
        { "sl", "Slovenian" },
        { "so", "Somali" },
        { "es", "Spanish" },
        { "su", "Sundanese" },
        { "sw", "Swahili" },
        { "sv", "Swedish" },
        { "tg", "Tajik" },
        { "ta", "Tamil" },
        { "tt", "Tatar" },
        { "te", "Telugu" },
        { "th", "Thai" },
        { "tr", "Turkish" },
        { "tk", "Turkmen" },
        { "uk", "Ukrainian" },
        { "ur", "Urdu" },
        { "ug", "Uyghur" },
        { "uz", "Uzbek" },
        { "vi", "Vietnamese" },
        { "cy", "Welsh" },
        { "xh", "Xhosa" },
        { "yi", "Yiddish" },
        { "yo", "Yoruba" },
        { "zu", "Zulu" }
    };

    static const inline QMap<QString, QString> m_regionLanguageMap = {
        { "af_ZA", "Afrikaans" },        // South Africa
        { "sq_AL", "Albanian" },         // Albania
        { "am_ET", "Amharic" },          // Ethiopia
        { "ar_SA", "Arabic" },           // Saudi Arabia
        { "hy_AM", "Armenian" },         // Armenia
        { "az_AZ", "Azerbaijani" },      // Azerbaijan
        { "eu_ES", "Basque" },           // Spain
        { "be_BY", "Belarusian" },       // Belarus
        { "bn_BD", "Bengali" },          // Bangladesh
        { "bs_BA", "Bosnian" },          // Bosnia and Herzegovina
        { "bg_BG", "Bulgarian" },        // Bulgaria
        { "ca_ES", "Catalan" },          // Spain
        { "ceb_PH", "Cebuano" },         // Philippines
        { "zh_CN", "Chinese" },          // China
        { "co_FR", "Corsican" },         // France
        { "hr_HR", "Croatian" },         // Croatia
        { "cs_CZ", "Czech" },            // Czech Republic
        { "da_DK", "Danish" },           // Denmark
        { "nl_NL", "Dutch" },            // Netherlands
        { "en_US", "English" },          // United States
        { "eo_EO", "Esperanto" },        // Esperanto (International)
        { "et_EE", "Estonian" },         // Estonia
        { "fil_PH", "Filipino" },        // Philippines
        { "fi_FI", "Finnish" },          // Finland
        { "fr_FR", "French" },           // France
        { "fy_NL", "Frisian" },          // Netherlands
        { "gl_ES", "Galician" },         // Spain
        { "ka_GE", "Georgian" },         // Georgia
        { "de_DE", "German" },           // Germany
        { "el_GR", "Greek" },            // Greece
        { "gu_IN", "Gujarati" },         // India
        { "ht_HT", "Haitian Creole" },   // Haiti
        { "ha_NG", "Hausa" },            // Nigeria
        { "haw_US", "Hawaiian" },        // United States
        { "he_IL", "Hebrew" },           // Israel
        { "hi_IN", "Hindi" },            // India
        { "hmn_CN", "Hmong" },           // China
        { "hu_HU", "Hungarian" },        // Hungary
        { "is_IS", "Icelandic" },        // Iceland
        { "ig_NG", "Igbo" },             // Nigeria
        { "id_ID", "Indonesian" },       // Indonesia
        { "ga_IE", "Irish" },            // Ireland
        { "it_IT", "Italian" },          // Italy
        { "ja_JP", "Japanese" },         // Japan
        { "jv_ID", "Javanese" },         // Indonesia
        { "kn_IN", "Kannada" },          // India
        { "kk_KZ", "Kazakh" },           // Kazakhstan
        { "km_KH", "Khmer" },            // Cambodia
        { "rw_RW", "Kinyarwanda" },      // Rwanda
        { "ko_KR", "Korean" },           // South Korea
        { "ku_TR", "Kurdish" },          // Turkey
        { "ky_KG", "Kyrgyz" },           // Kyrgyzstan
        { "lo_LA", "Lao" },              // Laos
        { "la_VA", "Latin" },            // Vatican
        { "lv_LV", "Latvian" },          // Latvia
        { "lt_LT", "Lithuanian" },       // Lithuania
        { "lb_LU", "Luxembourgish" },    // Luxembourg
        { "mk_MK", "Macedonian" },       // North Macedonia
        { "mg_MG", "Malagasy" },         // Madagascar
        { "ms_MY", "Malay" },            // Malaysia
        { "ml_IN", "Malayalam" },        // India
        { "mt_MT", "Maltese" },          // Malta
        { "mi_NZ", "Maori" },            // New Zealand
        { "mr_IN", "Marathi" },          // India
        { "mn_MN", "Mongolian" },        // Mongolia
        { "my_MM", "Myanmar (Burmese)" }, // Myanmar
        { "ne_NP", "Nepali" },           // Nepal
        { "no_NO", "Norwegian" },        // Norway
        { "ny_MW", "Nyanja (Chichewa)" }, // Malawi
        { "or_IN", "Odia (Oriya)" },     // India
        { "ps_AF", "Pashto" },           // Afghanistan
        { "fa_IR", "Persian" },          // Iran
        { "pl_PL", "Polish" },           // Poland
        { "pt_BR", "Portuguese" },       // Brazil
        { "pa_IN", "Punjabi" },          // India
        { "ro_RO", "Romanian" },         // Romania
        { "ru_RU", "Russian" },          // Russia
        { "sm_WS", "Samoan" },           // Samoa
        { "gd_GB", "Scots Gaelic" },     // United Kingdom
        { "sr_RS", "Serbian" },          // Serbia
        { "st_LS", "Sesotho" },          // Lesotho
        { "sn_ZW", "Shona" },            // Zimbabwe
        { "sd_PK", "Sindhi" },           // Pakistan
        { "si_LK", "Sinhala" },          // Sri Lanka
        { "sk_SK", "Slovak" },           // Slovakia
        { "sl_SI", "Slovenian" },        // Slovenia
        { "so_SO", "Somali" },           // Somalia
        { "es_ES", "Spanish" },          // Spain
        { "su_ID", "Sundanese" },        // Indonesia
        { "sw_KE", "Swahili" },          // Kenya
        { "sv_SE", "Swedish" },          // Sweden
        { "tg_TJ", "Tajik" },            // Tajikistan
        { "ta_IN", "Tamil" },            // India
        { "tt_RU", "Tatar" },            // Russia
        { "te_IN", "Telugu" },           // India
        { "th_TH", "Thai" },             // Thailand
        { "tr_TR", "Turkish" },          // Turkey
        { "tk_TM", "Turkmen" },          // Turkmenistan
        { "uk_UA", "Ukrainian" },        // Ukraine
        { "ur_PK", "Urdu" },             // Pakistan
        { "ug_CN", "Uyghur" },           // China
        { "uz_UZ", "Uzbek" },            // Uzbekistan
        { "vi_VN", "Vietnamese" },       // Vietnam
        { "cy_GB", "Welsh" },            // United Kingdom
        { "xh_ZA", "Xhosa" },            // South Africa
        { "yi_001", "Yiddish" },         // World (Global)
        { "yo_NG", "Yoruba" },           // Nigeria
        { "zu_ZA", "Zulu" }              // South Africa
    };

    static const inline QMap<QString, QString>  m_languageRegionMap = {
        { "Afrikaans", "af_ZA" },       // South Africa
        { "Albanian", "sq_AL" },        // Albania
        { "Amharic", "am_ET" },         // Ethiopia
        { "Arabic", "ar_SA" },          // Saudi Arabia
        { "Armenian", "hy_AM" },        // Armenia
        { "Azerbaijani", "az_AZ" },     // Azerbaijan
        { "Basque", "eu_ES" },          // Spain
        { "Belarusian", "be_BY" },      // Belarus
        { "Bengali", "bn_BD" },         // Bangladesh
        { "Bosnian", "bs_BA" },         // Bosnia and Herzegovina
        { "Bulgarian", "bg_BG" },       // Bulgaria
        { "Catalan", "ca_ES" },         // Spain
        { "Cebuano", "ceb_PH" },        // Philippines
        { "Chinese", "zh_CN" },         // China
        { "Corsican", "co_FR" },        // France
        { "Croatian", "hr_HR" },        // Croatia
        { "Czech", "cs_CZ" },           // Czech Republic
        { "Danish", "da_DK" },          // Denmark
        { "Dutch", "nl_NL" },           // Netherlands
        { "English", "en_US" },         // United States
        { "Esperanto", "eo_EO" },       // Esperanto (International)
        { "Estonian", "et_EE" },        // Estonia
        { "Filipino", "fil_PH" },       // Philippines
        { "Finnish", "fi_FI" },         // Finland
        { "French", "fr_FR" },          // France
        { "Frisian", "fy_NL" },         // Netherlands
        { "Galician", "gl_ES" },        // Spain
        { "Georgian", "ka_GE" },        // Georgia
        { "German", "de_DE" },          // Germany
        { "Greek", "el_GR" },           // Greece
        { "Gujarati", "gu_IN" },        // India
        { "Haitian Creole", "ht_HT" },  // Haiti
        { "Hausa", "ha_NG" },           // Nigeria
        { "Hawaiian", "haw_US" },       // United States
        { "Hebrew", "he_IL" },          // Israel
        { "Hindi", "hi_IN" },           // India
        { "Hmong", "hmn_CN" },          // China
        { "Hungarian", "hu_HU" },       // Hungary
        { "Icelandic", "is_IS" },       // Iceland
        { "Igbo", "ig_NG" },            // Nigeria
        { "Indonesian", "id_ID" },      // Indonesia
        { "Irish", "ga_IE" },           // Ireland
        { "Italian", "it_IT" },         // Italy
        { "Japanese", "ja_JP" },        // Japan
        { "Javanese", "jv_ID" },        // Indonesia
        { "Kannada", "kn_IN" },         // India
        { "Kazakh", "kk_KZ" },          // Kazakhstan
        { "Khmer", "km_KH" },           // Cambodia
        { "Kinyarwanda", "rw_RW" },     // Rwanda
        { "Korean", "ko_KR" },          // South Korea
        { "Kurdish", "ku_TR" },         // Turkey
        { "Kyrgyz", "ky_KG" },          // Kyrgyzstan
        { "Lao", "lo_LA" },             // Laos
        { "Latin", "la_VA" },           // Vatican
        { "Latvian", "lv_LV" },         // Latvia
        { "Lithuanian", "lt_LT" },      // Lithuania
        { "Luxembourgish", "lb_LU" },   // Luxembourg
        { "Macedonian", "mk_MK" },      // North Macedonia
        { "Malagasy", "mg_MG" },        // Madagascar
        { "Malay", "ms_MY" },           // Malaysia
        { "Malayalam", "ml_IN" },       // India
        { "Maltese", "mt_MT" },         // Malta
        { "Maori", "mi_NZ" },           // New Zealand
        { "Marathi", "mr_IN" },         // India
        { "Mongolian", "mn_MN" },       // Mongolia
        { "Myanmar (Burmese)", "my_MM" }, // Myanmar
        { "Nepali", "ne_NP" },          // Nepal
        { "Norwegian", "no_NO" },       // Norway
        { "Nyanja (Chichewa)", "ny_MW" }, // Malawi
        { "Odia (Oriya)", "or_IN" },    // India
        { "Pashto", "ps_AF" },          // Afghanistan
        { "Persian", "fa_IR" },         // Iran
        { "Polish", "pl_PL" },          // Poland
        { "Portuguese", "pt_BR" },      // Brazil
        { "Punjabi", "pa_IN" },         // India
        { "Romanian", "ro_RO" },        // Romania
        { "Russian", "ru_RU" },         // Russia
        { "Samoan", "sm_WS" },          // Samoa
        { "Scots Gaelic", "gd_GB" },    // United Kingdom
        { "Serbian", "sr_RS" },         // Serbia
        { "Sesotho", "st_LS" },         // Lesotho
        { "Shona", "sn_ZW" },           // Zimbabwe
        { "Sindhi", "sd_PK" },          // Pakistan
        { "Sinhala", "si_LK" },         // Sri Lanka
        { "Slovak", "sk_SK" },          // Slovakia
        { "Slovenian", "sl_SI" },       // Slovenia
        { "Somali", "so_SO" },          // Somalia
        { "Spanish", "es_ES" },         // Spain
        { "Sundanese", "su_ID" },       // Indonesia
        { "Swahili", "sw_KE" },         // Kenya
        { "Swedish", "sv_SE" },         // Sweden
        { "Tajik", "tg_TJ" },           // Tajikistan
        { "Tamil", "ta_IN" },           // India
        { "Tatar", "tt_RU" },           // Russia
        { "Telugu", "te_IN" },          // India
        { "Thai", "th_TH" },            // Thailand
        { "Turkish", "tr_TR" },         // Turkey
        { "Turkmen", "tk_TM" },         // Turkmenistan
        { "Ukrainian", "uk_UA" },       // Ukraine
        { "Urdu", "ur_PK" },            // Pakistan
        { "Uyghur", "ug_CN" },          // China
        { "Uzbek", "uz_UZ" },           // Uzbekistan
        { "Vietnamese", "vi_VN" },      // Vietnam
        { "Welsh", "cy_GB" },           // United Kingdom
        { "Xhosa", "xh_ZA" },           // South Africa
        { "Yiddish", "yi_001" },        // World (Global)
        { "Yoruba", "yo_NG" },          // Nigeria
        { "Zulu", "zu_ZA" }             // South Africa
    };

    static inline QMap<QString, QString> languageToFlagMap = {
        { "af", "za" }, // Afrikaans - Afrique du Sud
        { "sq", "al" }, // Albanian - Albanie
        { "am", "et" }, // Amharic - Éthiopie
        { "ar", "sa" }, // Arabic - Arabie Saoudite (parlé dans plusieurs pays)
        { "hy", "am" }, // Armenian - Arménie
        { "az", "az" }, // Azerbaijani - Azerbaïdjan
        { "eu", "es" }, // Basque - Espagne (région du Pays Basque)
        { "be", "by" }, // Belarusian - Biélorussie
        { "bn", "bd" }, // Bengali - Bangladesh
        { "bs", "ba" }, // Bosnian - Bosnie-Herzégovine
        { "bg", "bg" }, // Bulgarian - Bulgarie
        { "ca", "es" }, // Catalan - Espagne (région de la Catalogne)
        { "ceb", "ph" }, // Cebuano - Philippines
        { "zh", "cn" }, // Chinese - Chine
        { "co", "fr" }, // Corsican - France (région de Corse)
        { "hr", "hr" }, // Croatian - Croatie
        { "cs", "cz" }, // Czech - République Tchèque
        { "da", "dk" }, // Danish - Danemark
        { "nl", "nl" }, // Dutch - Pays-Bas
        { "en", "gb" }, // English - Royaume-Uni (parlé dans plusieurs pays)
        { "eo", "eu" }, // Esperanto - Pas de pays spécifique (souvent symbolisé par l'UE)
        { "et", "ee" }, // Estonian - Estonie
        { "fil", "ph" }, // Filipino - Philippines
        { "fi", "fi" }, // Finnish - Finlande
        { "fr", "fr" }, // French - France (parlé dans plusieurs pays)
        { "fy", "nl" }, // Frisian - Pays-Bas (Frise)
        { "gl", "es" }, // Galician - Espagne (région de Galice)
        { "ka", "ge" }, // Georgian - Géorgie
        { "de", "de" }, // German - Allemagne (parlé dans plusieurs pays)
        { "el", "gr" }, // Greek - Grèce
        { "gu", "in" }, // Gujarati - Inde
        { "ht", "ht" }, // Haitian Creole - Haïti
        { "ha", "ng" }, // Hausa - Nigeria
        { "haw", "us" }, // Hawaiian - États-Unis (Hawaï)
        { "he", "il" }, // Hebrew - Israël
        { "hi", "in" }, // Hindi - Inde
        { "hmn", "cn" }, // Hmong - Chine (et Asie du Sud-Est)
        { "hu", "hu" }, // Hungarian - Hongrie
        { "is", "is" }, // Icelandic - Islande
        { "ig", "ng" }, // Igbo - Nigeria
        { "id", "id" }, // Indonesian - Indonésie
        { "ga", "ie" }, // Irish - Irlande
        { "it", "it" }, // Italian - Italie
        { "ja", "jp" }, // Japanese - Japon
        { "jv", "id" }, // Javanese - Indonésie
        { "kn", "in" }, // Kannada - Inde
        { "kk", "kz" }, // Kazakh - Kazakhstan
        { "km", "kh" }, // Khmer - Cambodge
        { "rw", "rw" }, // Kinyarwanda - Rwanda
        { "ko", "kr" }, // Korean - Corée du Sud
        { "ku", "iq" }, // Kurdish - Irak (Kurdistan irakien)
        { "ky", "kg" }, // Kyrgyz - Kirghizistan
        { "lo", "la" }, // Lao - Laos
        { "la", "va" }, // Latin - Vatican (langue historique)
        { "lv", "lv" }, // Latvian - Lettonie
        { "lt", "lt" }, // Lithuanian - Lituanie
        { "lb", "lu" }, // Luxembourgish - Luxembourg
        { "mk", "mk" }, // Macedonian - Macédoine
        { "mg", "mg" }, // Malagasy - Madagascar
        { "ms", "my" }, // Malay - Malaisie
        { "ml", "in" }, // Malayalam - Inde
        { "mt", "mt" }, // Maltese - Malte
        { "mi", "nz" }, // Maori - Nouvelle-Zélande
        { "mr", "in" }, // Marathi - Inde
        { "mn", "mn" }, // Mongolian - Mongolie
        { "my", "mm" }, // Myanmar (Burmese) - Myanmar
        { "ne", "np" }, // Nepali - Népal
        { "no", "no" }, // Norwegian - Norvège
        { "ny", "mw" }, // Nyanja (Chichewa) - Malawi
        { "or", "in" }, // Odia (Oriya) - Inde
        { "ps", "af" }, // Pashto - Afghanistan
        { "fa", "ir" }, // Persian - Iran
        { "pl", "pl" }, // Polish - Pologne
        { "pt", "pt" }, // Portuguese - Portugal (ou "br" pour Brésil)
        { "pa", "in" }, // Punjabi - Inde (aussi Pakistan)
        { "ro", "ro" }, // Romanian - Roumanie
        { "ru", "ru" }, // Russian - Russie
        { "sm", "ws" }, // Samoan - Samoa
        { "gd", "gb" }, // Scots Gaelic - Royaume-Uni (Écosse)
        { "sr", "rs" }, // Serbian - Serbie
        { "st", "za" }, // Sesotho - Afrique du Sud
        { "sn", "zw" }, // Shona - Zimbabwe
        { "sd", "pk" }, // Sindhi - Pakistan
        { "si", "lk" }, // Sinhala - Sri Lanka
        { "sk", "sk" }, // Slovak - Slovaquie
        { "sl", "si" }, // Slovenian - Slovénie
        { "so", "so" }, // Somali - Somalie
        { "es", "es" }, // Spanish - Espagne (parlé dans plusieurs pays)
        { "su", "id" }, // Sundanese - Indonésie
        { "sw", "tz" }, // Swahili - Tanzanie (ou "ke" pour Kenya)
        { "sv", "se" }, // Swedish - Suède
        { "tg", "tj" }, // Tajik - Tadjikistan
        { "ta", "in" }, // Tamil - Inde (aussi Sri Lanka "lk")
        { "tt", "ru" }, // Tatar - Russie
        { "te", "in" }, // Telugu - Inde
        { "th", "th" }, // Thai - Thaïlande
        { "tr", "tr" }, // Turkish - Turquie
        { "tk", "tm" }, // Turkmen - Turkménistan
        { "uk", "ua" }, // Ukrainian - Ukraine
        { "ur", "pk" }, // Urdu - Pakistan
        { "ug", "cn" }, // Uyghur - Chine
        { "uz", "uz" }, // Uzbek - Ouzbékistan
        { "vi", "vn" }, // Vietnamese - Vietnam
        { "cy", "gb" }, // Welsh - Royaume-Uni (Pays de Galles)
        { "xh", "za" }, // Xhosa - Afrique du Sud
        { "yi", "il" }, // Yiddish - Israël (peut aussi être "pl" ou "de")
        { "yo", "ng" }, // Yoruba - Nigeria
        { "zu", "za" }  // Zulu - Afrique du Sud
    };;

};

#endif // FLAGSPICKERBOX_H
