#pragma once

namespace Exception {

class RuntimeErrorStack : public std::runtime_error {
public:
	
private:
	std::vector<std::unique_ptr<std::runtime_error>> stack_;
};

}
