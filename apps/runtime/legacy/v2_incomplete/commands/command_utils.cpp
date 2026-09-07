#include "commands/command_utils.h"

QMap<QString, CommandUtils::OptionDef> CommandUtils::s_optionsMap;
bool CommandUtils::s_initialized = false;

using namespace DialogCommandTokens;

void CommandUtils::init() {
  if (s_initialized)
    return;

  const struct {
    const char *optionKeyword;
    unsigned int optionCode;
    bool optionReset;
    bool commandFlag;
  } optionsParser[] = {
      {"checkable", PropertyCheckable, false, false},
      {"checked", PropertyChecked, false, false},
      {"text", PropertyText, false, false},
      {"title", PropertyTitle, false, false},
      {"password", PropertyPassword, false, false},
      {"placeholder", PropertyPlaceholder, false, false},
      {"icon", PropertyIcon, false, false},
      {"iconsize", PropertyIconSize, false, false},
      {"animation", PropertyAnimation, false, false},
      {"picture", PropertyPicture, false, false},
      {"apply", PropertyApply, false, false},
      {"exit", PropertyExit, false, false},
      {"default", PropertyDefault, false, false},
      {"space", OptionSpace, false, true},
      {"stretch", OptionStretch, false, true},
      {"behind", OptionBehind, false, true},
      {"onto", OptionOnto, false, true},
      {"enabled", OptionEnabled, false, true},
      {"focus", OptionFocus, false, true},
      {"stylesheet", OptionStyleSheet, false, true},
      {"visible", OptionVisible, false, true},
      {"horizontal", OptionVertical, true, true}, // Reset vertical
      {"vertical", OptionVertical, false, true},
      {"plain", PropertyPlain, false, false},
      {"raised", PropertyRaised, false, false},
      {"sunken", PropertySunken, false, false},
      {"flat", PropertyFlat, false, false},
      {"noframe", PropertyNoframe, false, false},
      {"box", PropertyBox, false, false},
      {"panel", PropertyPanel, false, false},
      {"styled", PropertyStyled, false, false},
      {"current", PropertyCurrent, false, false},
      {"activation", PropertyActivation, false, false},
      {"selection", PropertySelection, false, false},
      {"minimum", PropertyMinimum, false, false},
      {"maximum", PropertyMaximum, false, false},
      {"date", PropertyDate, false, false},
      {"format", PropertyFormat, false, false},
      {"navigation", PropertyNavigation, false, false},
      {"value", PropertyValue, false, false},
      {"busy", PropertyBusy, false, false},
      {"file", PropertyFile, false, false},
      {"top", PropertyPositionTop, false, false},
      {"bottom", PropertyPositionBottom, false, false},
      {"left", PropertyPositionLeft, false, false},
      {"right", PropertyPositionRight, false, false},
      {nullptr, 0, false, false}};

  for (int i = 0; optionsParser[i].optionKeyword; ++i) {
    s_optionsMap[optionsParser[i].optionKeyword] = {
        optionsParser[i].optionCode, optionsParser[i].optionReset,
        optionsParser[i].commandFlag};
  }

  // Also handle property variations for vertical/horizontal if they differ from
  // options? In old parser:
  // {"horizontal", OptionVertical, true, true},
  // {"horizontal", PropertyVertical, true, false},
  // {"vertical", OptionVertical, false, true},
  // {"vertical", PropertyVertical, false, false},
  // Using map overwrites duplicates. I need to handle this.
  // However, OptionVertical and PropertyVertical seem to collision in map key.
  // The old parser checked `commandFlag` vs context (stage).
  // Set command uses both?
  // "set [name] options"
  // Usually set applies to widget properties.
  // OptionVertical is for Step command?
  // PropertyVertical is for Widget properties.
  // If I am setting options on a widget, I care about Properties.
  // If I am setting options on Dialog (global?), maybe Options?

  // For SetCommand, we likely target widgets.
  // I will prioritize PropertyVertical for "horizontal"/"vertical".
  // Or add special logic.

  s_optionsMap["horizontal"] = {PropertyVertical, true, false};
  s_optionsMap["vertical"] = {PropertyVertical, false, false};

  s_initialized = true;
}

bool CommandUtils::getOption(const QString &keyword, OptionDef &def) {
  if (!s_initialized)
    init();
  if (s_optionsMap.contains(keyword)) {
    def = s_optionsMap[keyword];
    return true;
  }
  return false;
}
