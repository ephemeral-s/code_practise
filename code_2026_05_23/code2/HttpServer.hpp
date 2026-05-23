#pragma once
#include "Socket.hpp"
#include "TcpServer.hpp"
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <memory>
#include "Util.hpp"

const std::string webroot = "./www";
const std::string homepage = "/index.html";
const std::string page_404 = "/404.html";

class HttpRequest {
public:
    //解析请求行
    void ParseReqLine(const std::string& s) {
        std::stringstream ss(s);
        ss >> _method >> _uri >> _version;
    }

    //反序列化
    void Deserialize(std::string& reqstr) {
        //获取请求行
        std::string reqline;
        Util::ReadOneLine(reqstr, reqline, _endline);

        //对请求行进行反序列化
        ParseReqLine(reqline);

        //处理uri转化为本地路径
        if(_uri == "/") _uri = webroot + homepage;
        else _uri = webroot + _uri;

        //提取查询参数
        size_t pos = _uri.find('?');
        if(pos != std::string::npos) {
            _args = _uri.substr(pos + 1);
            _uri = _uri.substr(0, pos);
            _isexec = true;
        }
    }

    std::string Uri()
    {
        return _uri;
    }

    bool Isexec()
    {
        return _isexec;
    }

    std::string Args()
    {
        return _args;
    }
private:
    std::string _method;       //请求方法
    std::string _uri;          //资源路径
    std::string _version;      //版本
    std::unordered_map<std::string, std::string> _headers; //请求报头
    const std::string _endline = "\r\n"; // 空行
    std::string _text;         //请求正文
    std::string _args;         //查询参数
    bool _isexec = false;      //是否动态交互
};

class HttpResponse
{
public:
    //序列化
    std::string Serialize()
    {
        std::string repstr = _version + " " + std::to_string(_code) + " " + _desc + _endline;
        for(auto& header : _headers)
        {
            repstr += header.first + ": " + header.second + _endline;
        }
        repstr += _endline;
        repstr += _text;
        return repstr;
    }

    void SetTargetFile(const std::string& target)
    {
        _targetfile = target;
    }

    void SetCode(int code)
    {
        switch (code)
        {
        case 200:
            _code = 200;
            _desc = "OK";
            break;
        case 404:
            _code = 404;
            _desc = "Not Found";
            break;
        default:
            break;
        }
    }

    void SetHeader(const std::string key, const std::string value)
    {
        if(_headers.find(key) != _headers.end()) return; // 如果有就不插入
        _headers[key] = value;
    }

    std::string Uri2Suffix()
    {
        auto pos = _targetfile.rfind('.');
        if(pos != std::string::npos)
        {
            std::string suffix = _targetfile.substr(pos + 1);

            if(suffix == "html" || suffix == "htm") return "text/html";
            else if(suffix == "jpg") return "image/jpeg";
            else if(suffix == "png") return "image/png";
        }
        return "";
    }

    void SetText(const std::string& text)
    {
        _text = text;
    }

    //构建应答
    void MakeResponse()
    {
        ssize_t filesize = 0;
        //读取网页资源
        if(!Util::ReadFileContent(_targetfile, _text))
        {
            LOG_WARN("客户端获取", _targetfile, "但不存在");
            SetCode(404);
            //重新读取页面 -- 404页面
            _targetfile = webroot + page_404;
            Util::ReadFileContent(_targetfile, _text);
        }
        else
        {
            SetCode(200);
        }
        filesize = Util::FileSize(_targetfile);
        SetHeader("Content_Length", std::to_string(filesize));

        std::string suffix = Uri2Suffix();
        if(suffix != "")
        {
            SetHeader("Content_Type", suffix);
        }
    }

private:
    std::string _version = "HTTP/1.1"; // 版本
    int _code; // 状态码
    std::string _desc; // 状态码描述
    std::unordered_map<std::string, std::string> _headers; // 报头
    const std::string _endline = "\r\n"; // 空行
    std::string _text;  // 正文

    std::string _targetfile; // 目标页面
};

class HttpServer {
public:
    HttpServer(uint16_t port) : _port(port) {
        _tsvr = std::make_unique<TcpServer>(port);
    }

    void HanderRequest(std::shared_ptr<Socket> sock, const std::string& ip, uint16_t port) {
        
    }

    void Start() {

    }
private:
    uint16_t _port;
    std::unique_ptr<TcpServer> _tsvr = nullptr;
};