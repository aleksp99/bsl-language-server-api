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

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/regex.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "AddInNative.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string AddInNative::extensionName() {
    return "BSLLanguageServerAPI";
}

std::string AddInNative::wcharToMultiByte(const std::wstring& wstr)
{
    DWORD locale = CP_UTF8;
    if (wstr.empty()) return {};
    const int sz = WideCharToMultiByte(locale, 0, &wstr[0], (int)wstr.size(), 0, 0, 0, 0);
    std::string res(sz, 0);
    WideCharToMultiByte(locale, 0, &wstr[0], (int)wstr.size(), &res[0], sz, 0, 0);
    return res;
}

std::wstring AddInNative::multiByteToWchar(const std::string& str)
{
    DWORD locale = CP_UTF8;
    if (str.empty()) return {};
    const int sz = MultiByteToWideChar(locale, 0, &str[0], (int)str.size(), 0, 0);
    std::wstring res(sz, 0);
    MultiByteToWideChar(locale, 0, &str[0], (int)str.size(), &res[0], sz);
    return res;
}

AddInNative::AddInNative() {
    
    // Full featured property registration example
    AddProperty(L"Version", L"ВерсияКомпоненты", [&]() {return std::make_shared<variant_t>(std::move(std::string(Version))); });

    AddProperty(L"URL", L"Адрес",
        [&]() {return std::make_shared<variant_t>(std::move(std::string(url))); },
        [&](variant_t value) {
            std::wstring wURL = std::get<const wchar_t*>(value);
            url = std::string(wURL.begin(), wURL.end());
        });
    
    AddMethod(L"Check", L"Проверять", this, &AddInNative::check);
}

struct {
    double jsonrpc = 2.0;
    std::string method = "textDocument/didOpen";
    struct {
        struct {
            std::string uri = "inmemory://model/1";
            std::string languageId = "bsl";
            int version = 1;
            std::wstring text;
        } textDocument;
    } params;
} didOpen;

struct {
    double jsonrpc = 2.0;
    std::string method = "textDocument/didOpen";
    struct {
        struct {
            std::string uri = "inmemory://model/1";
        } textDocument;
    } params;
} didClose;

variant_t AddInNative::check(variant_t text) {
    
    try
    {
        boost::regex ex("(ws)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)");
        boost::cmatch what;
        if (!regex_match(url.c_str(), what, ex)) {
            std::stringstream ss;
            return std::string("Wrong URL: " + url);
        }
        std::string host = std::string(what[2].first, what[2].second);
        std::string port = std::string(what[3].first, what[3].second);
        std::string path = std::string(what[4].first, what[4].second);

        std::wstring wDidOpen = L"{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/didOpen\",\"params\":{\"textDocument\":{\"uri\":\"inmemory://model/1\",\"languageId\":\"bsl\",\"version\":1,\"text\":\"" + std::wstring(std::get<const wchar_t*>(text))  + L"\"}}}";
        std::string didOpen = wcharToMultiByte(wDidOpen);
        std::string didClose = "{\"jsonrpc\":\"2.0\",\"method\":\"textDocument/didClose\",\"params\":{\"textDocument\":{\"uri\":\"inmemory://model/1\"}}}";

        // io_context требуется для всех операций ввода-вывода.
        net::io_context ioc;

        // Эти объекты выполняют наш ввод-вывод.
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        // Найдите доменное имя
        auto const results = resolver.resolve(host, port);

        // Делаем соединение по IP-адресу, который получаем из поиска
        net::connect(ws.next_layer(), results.begin(), results.end());

        // Установите декоратор, чтобы изменить пользовательский агент рукопожатия
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-coro");
            }));

        // Выполните рукопожатие через веб-сокет
        ws.handshake(host, path);

        // Отправить сообщение
        ws.write(net::buffer(didOpen));
        ws.write(net::buffer(didClose));

        // Этот буфер будет содержать входящее сообщение
        beast::flat_buffer buffer;

        // Прочитать сообщение в наш буфер
        ws.read(buffer);

        // Закройте соединение WebSocket
        ws.close(websocket::close_code::normal);

        std::stringstream ss;
        ss << beast::make_printable(buffer.data());
        return ss.str();
    }
    catch (std::exception const& e) {
        return e.what();
    }
    return "";
}
