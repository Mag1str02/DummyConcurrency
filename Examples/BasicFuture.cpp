
#include "DummyConcurrency/DummyConcurrency.hpp"

#include <expected>
#include <format>

using namespace std::chrono_literals;
using namespace NDummyConcurrency;
using namespace NDummyConcurrency;

TryFuture<int>         AcceptConnection();
TryFuture<std::string> ReadSocket(int fd);
void                   Print(std::string path);

void FutureDefault() {
    auto socket = Get(AcceptConnection());
    if (socket.has_value()) {
        auto data = Get(ReadSocket(socket.value()));
        if (data.has_value()) {
            Print(data.value());
        } else {
            throw std::exception();
        }
    } else {
        throw std::exception();
    }
}

void FutureCombinators() {
    Status result = AcceptConnection()                                         //
                    | NFuture::AndThen([](int fd) { return ReadSocket(fd); })  //
                    | NFuture::MapOk([](std::string&& data) {
                          Print(data);
                          return Unit();
                      })  //
                    | NFuture::Get();
    if (!result.has_value()) {
        throw std::exception();
    }
}

int main() {
    FutureCombinators();
}

TryFuture<int> AcceptConnection() {
    return NFuture::Ok(1);
}
TryFuture<std::string> ReadSocket(int fd) {
    return NFuture::Ok(std::format("Data from socket {}", fd));
}
void Print(std::string value) {
    std::println("{}", value);
}