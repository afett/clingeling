/*
   Copyright (c) 2023 Andreas Fett
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <sys/types.h>
#include <map>
#include <memory>
#include <string>

namespace UDev {

class Device {
public:
	virtual std::shared_ptr<Device> parent() const = 0;
	virtual std::shared_ptr<Device> parent_with_subsystem_devtype(std::string const& subsystem, std::string const& devtype) const = 0;

	virtual std::string devpath() const = 0;
	virtual std::string subsystem() const = 0;
	virtual std::string devtype() const = 0;
	virtual std::string syspath() const = 0;
	virtual std::string sysname() const = 0;
	virtual std::string sysnum() const = 0;
	virtual std::string devnode() const = 0;

	virtual bool is_initialized() const = 0;
	virtual std::map<std::string, std::string> devlinks() const = 0;
	virtual std::map<std::string, std::string> properties() const = 0;
	virtual std::map<std::string, std::string> tags() const = 0;
	virtual std::map<std::string, std::string> current_tags() const = 0;
	virtual std::map<std::string, std::string> sysattr() const = 0;
	virtual std::string property_value(std::string const& key) const = 0;
	virtual std::string driver() const = 0;
	virtual dev_t devnum() const = 0;
	virtual std::string action() const = 0;
	virtual unsigned long long int seqnum() const = 0;
	virtual unsigned long long int usec_since_initialized() const = 0;

	virtual std::string sysattr_value(std::string const& sysattr) const = 0;
	virtual void sysattr_value(std::string const& sysattr, std::string const& value) = 0;
	virtual bool has_tag(std::string const& tag) const = 0;
	virtual bool has_current_tag(std::string const& tag) const = 0;
};

class Monitor {
public:
	virtual void enable_receiving() = 0;
	virtual void receive_buffer_size(int size) = 0;
	virtual int fd() const = 0;
	virtual std::shared_ptr<Device> receive_device() const = 0;

	virtual void filter_add_match_subsystem_devtype(std::string const& subsystem, std::string const& devtype) = 0;
	virtual void filter_add_match_tag(std::string const& tag) = 0;
	virtual void filter_update() = 0;
	virtual void filter_remove() = 0;
};

class Enumerate {
public:
	virtual void add_match_subsystem(std::string const& subsystem) = 0;
	virtual void add_nomatch_subsystem(std::string const& subsystem) = 0;
	virtual void add_match_sysattr(std::string const& sysattr, std::string const& value) = 0;
	virtual void add_nomatch_sysattr(std::string const& sysattr, std::string const& value) = 0;
	virtual void add_match_property(std::string const& property, std::string const& value) = 0;
	virtual void add_match_sysname(std::string const& sysname) = 0;
	virtual void add_match_tag(std::string const& tag) = 0;
	virtual void add_match_parent(std::shared_ptr<Device> const& parent) = 0;
	virtual void add_match_is_initialized() = 0;
	virtual void add_syspath(std::string const& syspath) = 0;
	virtual void scan_devices() = 0;
	virtual void scan_subsystems() = 0;
	virtual std::map<std::string, std::string> get() const = 0;
};

class Factory {
public:
	static std::shared_ptr<Factory> make();

	virtual std::shared_ptr<Device> make_device_from_syspath(std::string const& syspath) = 0;
	virtual std::shared_ptr<Device> make_device_from_devnum(char type, dev_t devnum) = 0;
	virtual std::shared_ptr<Device> make_device_from_subsystem_sysname(std::string const& subsystem, std::string const& sysname) = 0;
	virtual std::shared_ptr<Device> make_device_from_device_id(std::string const& id) = 0;
	virtual std::shared_ptr<Device> make_device_from_environment() = 0;

	virtual std::shared_ptr<Monitor> make_monitor(std::string const& name) = 0;

	virtual std::shared_ptr<Enumerate> make_enumerate() = 0;
};

}
