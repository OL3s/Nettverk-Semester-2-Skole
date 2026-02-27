#include "httplib.h"

#include <iostream>

namespace httpTask {
    void RegisterRoutes(httplib::Server& server) {
        server.Get("/", [](const httplib::Request&, httplib::Response& response) {
        response.set_content(R"(<!doctype html>
        <html>
        <head>
            <meta charset="utf-8" />
            <title>Routes</title>
            <style>
            body { font-family: Arial, sans-serif; padding: 24px; }
            .container { max-width: 420px; margin: 0 auto; }
            .btn {
                width: 100%;
                padding: 12px 16px;
                margin: 8px 0;
                font-size: 16px;
                cursor: pointer;
            }
            </style>
        </head>
        <body>
            <div class="container">
            <h1>Routes</h1>

            <form action="/heil" method="get"><button class="btn" type="submit">/heil</button></form>
            <form action="/gooner" method="get"><button class="btn" type="submit">/gooner</button></form>
            <form action="/gogogaga" method="get"><button class="btn" type="submit">/gogogaga</button></form>
            </div>
        </body>
        </html>
        )", "text/html");
        });

        server.Get("/heil", [](const httplib::Request&, httplib::Response& response) {
            response.set_content(
                "<html><body><image src=\"https://gifdb.com/images/high/eric-cartman-nazi-hitler-2ojv6w251ywpb7u8.webp\"></body></html>"
                ,"text/html");
        });

        server.Get("/gooner", [](const httplib::Request&, httplib::Response& response) {
            response.set_content(
                "<html><body><image src=\"https://media.tenor.com/nXd6vrqtrnYAAAAi/kermit-the-frog.gif\"></body></html>"
                ,"text/html");
        });

        server.Get("/gogogaga", [](const httplib::Request&, httplib::Response& response) {
            response.set_content("<html><body><image src=\"https://media.tenor.com/ST3HJk_ML3QAAAAi/go-on.gif\"></body></html>", "text/html");
        });
    }
}

int main() {
    // HTTPS/TLS (cpp-httplib) is enabled by compiling with:
    //   -DCPPHTTPLIB_OPENSSL_SUPPORT -lssl -lcrypto
    // and providing a certificate + private key.
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    const char* cert_path = "./cert.pem";
    const char* key_path = "./key.pem";

    httplib::SSLServer server(cert_path, key_path);
    if (!server.is_valid()) {
        std::cerr << "Failed to start TLS server. Ensure cert.pem/key.pem exist and are valid.\n";
        return 1;
    }

    httpTask::RegisterRoutes(server);
    std::cout << "Listening on https://127.0.0.1:8443\n";
    server.listen("127.0.0.1", 8443);
#else
    httplib::Server server;
    httpTask::RegisterRoutes(server);

    std::cout << "Listening on http://127.0.0.1:8080\n";
    server.listen("127.0.0.1", 8080);
#endif
    return 0;
}