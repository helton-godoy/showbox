#ifndef COMMAND_UTILS_H
#define COMMAND_UTILS_H

#include "showbox.h"
#include <QMap>
#include <QString>

class CommandUtils {
public:
  struct OptionDef {
    unsigned int code;
    bool reset;
    bool commandFlag;
  };

  static void init();
  static bool getOption(const QString &keyword, OptionDef &def);

private:
  static QMap<QString, OptionDef> s_optionsMap;
  static bool s_initialized;
};

#endif // COMMAND_UTILS_H
