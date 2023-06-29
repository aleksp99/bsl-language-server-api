/*
 *  Modern Native AddIn
 *  Copyright (C) 2018  Infactum
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef ADDINNATIVE_H
#define ADDINNATIVE_H

#include "Component.h"

class AddInNative final : public Component {
public:
    const char *Version = u8"1.0.1";

    AddInNative();

private:
    std::string extensionName() override;
    std::wstring url;

    std::string wcharToMultiByte(const std::wstring& wstr);
    std::wstring multiByteToWchar(const std::string& str);

    variant_t check(variant_t& text);
};

#endif //ADDINNATIVE_H
