#include "base_object.hpp"

#include "symbol.hpp"
#include "embeded_object.hpp"

namespace nscheme {

std::string ObjectToString(ObjectRef obj) {
    if (IsInteger(obj))
        return std::to_string(GetIntegerValue(obj));
    if (IsNil(obj))
        return "()";
    if (IsBoolean(obj)) {
        if (GetBooleanValue(obj))
            return "#t";
        else
            return "#f";
    }
    if (IsSymbol(obj))
        return GetSymbolValue(obj).ToString();
    if (IsCharacter(obj)) {
        char value = GetCharacterValue(obj);
        if (isalnum(static_cast<unsigned char>(value))) {
            return "#\\" + std::string(1, value);
        } else {
            char buffer[16];
            std::snprintf(buffer, sizeof(buffer), "#\\x%02x", value);
            return std::string(buffer);
        }
    }
    return reinterpret_cast<const Object*>(obj)->ToString();
}

}
