#include <udev.h>
#include <libudev.h>
#include <stdexcept>

#include "posix/system-error.h"

namespace {

template <typename T>
using unref_unique_ptr = std::unique_ptr<T, T*(*)(T *)>;

template <typename T, typename F>
unref_unique_ptr<T> make_unref_unique_ptr(T* obj, F* deleter)
{
	return unref_unique_ptr<T>{obj, deleter};
}

std::map<std::string, std::string> copy_udev_list(udev_list_entry *first_entry)
{
	if (!first_entry) {
		return {};
	}

	auto res = std::map<std::string, std::string>{};
	auto list_entry = first_entry;
	udev_list_entry_foreach(list_entry, first_entry) {
		auto name = udev_list_entry_get_name(list_entry);
		if (!name) {
			continue;
		}
		auto value = udev_list_entry_get_value(list_entry);
		res.insert({std::string{name}, std::string{value ? value : ""}});
	}
	return res;
}

}

namespace UDev {

class DeviceImpl : public Device {
public:
	explicit DeviceImpl(unref_unique_ptr<udev_device>);

	udev_device *raw() const;

	std::shared_ptr<Device> parent() const final;
	std::shared_ptr<Device> parent_with_subsystem_devtype(std::string const& subsystem, std::string const& devtype) const final;

	std::string devpath() const final;
	std::string subsystem() const final;
	std::string devtype() const final;
	std::string syspath() const final;
	std::string sysname() const final;
	std::string sysnum() const final;
	std::string devnode() const final;

	bool is_initialized() const final;
	std::map<std::string, std::string> devlinks() const final;
	std::map<std::string, std::string> properties() const final;
	std::map<std::string, std::string> tags() const final;
	std::map<std::string, std::string> current_tags() const final;
	std::map<std::string, std::string> sysattr() const final;
	std::string property_value(std::string const& key) const final;
	std::string driver() const final;
	dev_t devnum() const final;
	std::string action() const final;
	unsigned long long int seqnum() const final;
	unsigned long long int usec_since_initialized() const final;

	std::string sysattr_value(std::string const& sysattr) const final;
	void sysattr_value(std::string const& sysattr, std::string const& value) final;
	bool has_tag(std::string const& tag) const final;
	bool has_current_tag(std::string const& tag) const final;

private:
	unref_unique_ptr<udev_device> raw_;
};

DeviceImpl::DeviceImpl(unref_unique_ptr<udev_device> raw)
:
	raw_(std::move(raw))
{ }

udev_device *DeviceImpl::raw() const
{
	return raw_.get();
}

std::shared_ptr<Device> DeviceImpl::parent() const
{
	auto raw = udev_device_get_parent(raw_.get());
	if (!raw) {
		throw std::bad_alloc();
	}
	auto dev = make_unref_unique_ptr(udev_device_ref(raw), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Device> DeviceImpl::parent_with_subsystem_devtype(std::string const& subsystem, std::string const& devtype) const
{
	auto raw = udev_device_get_parent_with_subsystem_devtype(raw_.get(), subsystem.c_str(), devtype.c_str());
	if (!raw) {
		throw std::bad_alloc();
	}
	auto dev = make_unref_unique_ptr(udev_device_ref(raw), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::string DeviceImpl::devpath() const
{
	auto res = udev_device_get_devpath(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_devpath(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::subsystem() const
{
	auto res = udev_device_get_subsystem(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_subsystem(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::devtype() const
{
	auto res = udev_device_get_devtype(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_devtype(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::syspath() const
{
	auto res = udev_device_get_syspath(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_syspath(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::sysname() const
{
	auto res = udev_device_get_sysname(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_sysname(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::sysnum() const
{
	auto res = udev_device_get_sysnum(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_sysnum(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::devnode() const
{
	auto res = udev_device_get_devnode(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_devnode(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

bool DeviceImpl::is_initialized() const
{
	auto err = udev_device_get_is_initialized(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_device_get_is_initialized(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
	return err > 0;
}

std::map<std::string, std::string> DeviceImpl::devlinks() const
{
	return copy_udev_list(udev_device_get_devlinks_list_entry(raw_.get()));
}

std::map<std::string, std::string> DeviceImpl::properties() const
{
	return copy_udev_list(udev_device_get_properties_list_entry(raw_.get()));
}

std::map<std::string, std::string> DeviceImpl::tags() const
{
	return copy_udev_list(udev_device_get_tags_list_entry(raw_.get()));
}

std::map<std::string, std::string> DeviceImpl::current_tags() const
{
	return copy_udev_list(udev_device_get_current_tags_list_entry(raw_.get()));
}

std::map<std::string, std::string> DeviceImpl::sysattr() const
{
	return copy_udev_list(udev_device_get_sysattr_list_entry(raw_.get()));
}

std::string DeviceImpl::property_value(std::string const& key) const
{
	auto res = udev_device_get_property_value(raw_.get(), key.c_str());
	return res ? res : "";
}

std::string DeviceImpl::driver() const
{
	auto res = udev_device_get_driver(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_driver(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

dev_t DeviceImpl::devnum() const
{
	auto res = udev_device_get_devnum(raw_.get());
	if (::major(res) == 0 && ::minor(res) == 0) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_devnum(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

std::string DeviceImpl::action() const
{
	auto res = udev_device_get_driver(raw_.get());
	if (!res) {
		throw std::runtime_error(Fmt::format("%s: udev_device_get_driver(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return res;
}

unsigned long long int DeviceImpl::seqnum() const
{
	return udev_device_get_seqnum(raw_.get());
}

unsigned long long int DeviceImpl::usec_since_initialized() const
{
	return udev_device_get_usec_since_initialized(raw_.get());
}

std::string DeviceImpl::sysattr_value(std::string const& sysattr) const
{
	auto res = udev_device_get_sysattr_value(raw_.get(), sysattr.c_str());
	return res ? res : "";
}

void DeviceImpl::sysattr_value(std::string const& sysattr, std::string const& value)
{
	auto err = udev_device_set_sysattr_value(raw_.get(), sysattr.c_str(), value.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_device_set_sysattr_value(%x, %s, %s)",
				to_string(SourceLocation::current()), raw_.get(), sysattr, value);
	}
}

bool DeviceImpl::has_tag(std::string const& tag) const
{
	auto err = udev_device_has_tag(raw_.get(), tag.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_device_has_tag(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), tag);
	}
	return err > 0;
}

bool DeviceImpl::has_current_tag(std::string const& tag) const
{
	auto err = udev_device_has_current_tag(raw_.get(), tag.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_device_has_current_tag(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), tag);
	}
	return err > 0;
}

class MonitorImpl : public Monitor {
public:
	explicit MonitorImpl(unref_unique_ptr<udev_monitor>);

	void enable_receiving() final;
	void receive_buffer_size(int size) final;
	int fd() const final;
	std::shared_ptr<Device> receive_device() const final;

	void filter_add_match_subsystem_devtype(std::string const& subsystem, std::string const& devtype) final;
	void filter_add_match_tag(std::string const& tag) final;
	void filter_update() final;
	void filter_remove() final;

private:
	unref_unique_ptr<udev_monitor> raw_;
};

MonitorImpl::MonitorImpl(unref_unique_ptr<udev_monitor> raw)
:
	raw_(std::move(raw))
{ }

void MonitorImpl::enable_receiving()
{
	auto err = udev_monitor_enable_receiving(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_enable_receiving(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

void MonitorImpl::receive_buffer_size(int size)
{
	auto err = udev_monitor_set_receive_buffer_size(raw_.get(), size);
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_set_receive_buffer_size(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), size);
	}
}

int MonitorImpl::fd() const
{
	auto fd = udev_monitor_get_fd(raw_.get());
	if (fd < 0) {
		throw Posix::make_system_error(-fd, "%s: udev_monitor_get_fd(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
	return fd;
}

std::shared_ptr<Device> MonitorImpl::receive_device() const
{
	auto dev = make_unref_unique_ptr(udev_monitor_receive_device(raw_.get()), &udev_device_unref);

	if (!dev) {
		throw std::runtime_error(Fmt::format("%s: udev_monitor_receive_device(%x)",
					to_string(SourceLocation::current()), raw_.get()));
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

void MonitorImpl::filter_add_match_subsystem_devtype(std::string const& subsystem, std::string const& devtype)
{
	auto err = udev_monitor_filter_add_match_subsystem_devtype(raw_.get(), subsystem.c_str(), devtype.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_filter_add_match_subsystem_devtype(%x, %s, %s)",
				to_string(SourceLocation::current()), raw_.get(), subsystem, devtype);
	}
}

void MonitorImpl::filter_add_match_tag(std::string const& tag)
{
	auto err = udev_monitor_filter_add_match_tag(raw_.get(), tag.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_filter_add_match_tag(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), tag);
	}
}

void MonitorImpl::filter_update()
{
	auto err = udev_monitor_filter_update(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_filter_update(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

void MonitorImpl::filter_remove()
{
	auto err = udev_monitor_filter_remove(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_monitor_filter_remove(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

class EnumerateImpl : public Enumerate {
public:
	explicit EnumerateImpl(unref_unique_ptr<udev_enumerate>);

	void add_match_subsystem(std::string const& subsystem) final;
	void add_nomatch_subsystem(std::string const& subsystem) final;
	void add_match_sysattr(std::string const& sysattr, std::string const& value) final;
	void add_nomatch_sysattr(std::string const& sysattr, std::string const& value) final;
	void add_match_property(std::string const& property, std::string const& value) final;
	void add_match_sysname(std::string const& sysname) final;
	void add_match_tag(std::string const& tag) final;
	void add_match_parent(std::shared_ptr<Device> const& parent) final;
	void add_match_is_initialized() final;
	void add_syspath(std::string const& syspath) final;
	void scan_devices() final;
	void scan_subsystems() final;
	std::map<std::string, std::string> get() const final;

private:
	unref_unique_ptr<udev_enumerate> raw_;
};

EnumerateImpl::EnumerateImpl(unref_unique_ptr<udev_enumerate> raw)
:
	raw_(std::move(raw))
{ }

void EnumerateImpl::add_match_subsystem(std::string const& subsystem)
{
	auto err = udev_enumerate_add_match_subsystem(raw_.get(), subsystem.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_subsystem(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), subsystem);
	}
}

void EnumerateImpl::add_nomatch_subsystem(std::string const& subsystem)
{
	auto err = udev_enumerate_add_nomatch_subsystem(raw_.get(), subsystem.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_nomatch_subsystem(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), subsystem);
	}
}

void EnumerateImpl::add_match_sysattr(std::string const& sysattr, std::string const& value)
{
	auto err = udev_enumerate_add_match_sysattr(raw_.get(), sysattr.c_str(), value.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_sysattr(%x, %s, %s)",
				to_string(SourceLocation::current()), raw_.get(), sysattr, value);
	}
}

void EnumerateImpl::add_nomatch_sysattr(std::string const& sysattr, std::string const& value)
{
	auto err = udev_enumerate_add_nomatch_sysattr(raw_.get(), sysattr.c_str(), value.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_nomatch_sysattr(%x, %s, %s)",
				to_string(SourceLocation::current()), raw_.get(), sysattr, value);
	}
}

void EnumerateImpl::add_match_property(std::string const& property, std::string const& value)
{
	auto err = udev_enumerate_add_match_property(raw_.get(), property.c_str(), value.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_property(%x, %s, %s)",
				to_string(SourceLocation::current()), raw_.get(), property, value);
	}
}

void EnumerateImpl::add_match_sysname(std::string const& sysname)
{
	auto err = udev_enumerate_add_match_sysname(raw_.get(), sysname.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_sysname(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), sysname);
	}
}

void EnumerateImpl::add_match_tag(std::string const& tag)
{
	auto err = udev_enumerate_add_match_tag(raw_.get(), tag.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_tag(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), tag);
	}
}

void EnumerateImpl::add_match_parent(std::shared_ptr<Device> const& parent)
{
	auto err = udev_enumerate_add_match_parent(raw_.get(), static_cast<DeviceImpl*>(parent.get())->raw());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_parent(%x, %x)",
				to_string(SourceLocation::current()), raw_.get(), parent.get());
	}
}

void EnumerateImpl::add_match_is_initialized()
{
	auto err = udev_enumerate_add_match_is_initialized(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_match_is_initialized(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

void EnumerateImpl::add_syspath(std::string const& syspath)
{
	auto err = udev_enumerate_add_syspath(raw_.get(), syspath.c_str());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_add_syspath(%x, %s)",
				to_string(SourceLocation::current()), raw_.get(), syspath);
	}
}

void EnumerateImpl::scan_devices()
{
	auto err = udev_enumerate_scan_devices(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_scan_devices(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

void EnumerateImpl::scan_subsystems()
{
	auto err = udev_enumerate_scan_subsystems(raw_.get());
	if (err < 0) {
		throw Posix::make_system_error(-err, "%s: udev_enumerate_scan_subsystems(%x)",
				to_string(SourceLocation::current()), raw_.get());
	}
}

std::map<std::string, std::string> EnumerateImpl::get() const
{
	return copy_udev_list(udev_enumerate_get_list_entry(raw_.get()));
}

class FactoryImpl : public Factory {
public:
	explicit FactoryImpl(unref_unique_ptr<udev>);

	std::shared_ptr<Device> make_device_from_syspath(std::string const& syspath) final;
	std::shared_ptr<Device> make_device_from_devnum(char type, dev_t devnum) final;
	std::shared_ptr<Device> make_device_from_subsystem_sysname(std::string const& subsystem, std::string const& sysname) final;
	std::shared_ptr<Device> make_device_from_device_id(std::string const& id) final;
	std::shared_ptr<Device> make_device_from_environment() final;

	std::shared_ptr<Monitor> make_monitor(std::string const& name) final;

	std::shared_ptr<Enumerate> make_enumerate() final;

private:
	unref_unique_ptr<udev> raw_;
};

FactoryImpl::FactoryImpl(unref_unique_ptr<udev> raw)
:
	raw_(std::move(raw))
{ }

std::shared_ptr<Device> FactoryImpl::make_device_from_syspath(std::string const& syspath)
{
	auto dev = make_unref_unique_ptr(udev_device_new_from_syspath(raw_.get(), syspath.c_str()), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Device> FactoryImpl::make_device_from_devnum(char type, dev_t devnum)
{
	auto dev = make_unref_unique_ptr(udev_device_new_from_devnum(raw_.get(), type, devnum), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Device> FactoryImpl::make_device_from_subsystem_sysname(std::string const& subsystem, std::string const& sysname)
{
	auto dev = make_unref_unique_ptr(udev_device_new_from_subsystem_sysname(raw_.get(), subsystem.c_str(), sysname.c_str()), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Device> FactoryImpl::make_device_from_device_id(std::string const& id)
{
	auto dev = make_unref_unique_ptr(udev_device_new_from_device_id(raw_.get(), id.c_str()), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Device> FactoryImpl::make_device_from_environment()
{
	auto dev = make_unref_unique_ptr(udev_device_new_from_environment(raw_.get()), &udev_device_unref);
	if (!dev) {
		throw std::bad_alloc();
	}
	return std::make_shared<DeviceImpl>(std::move(dev));
}

std::shared_ptr<Monitor> FactoryImpl::make_monitor(std::string const& name)
{
	auto monitor = make_unref_unique_ptr(udev_monitor_new_from_netlink(raw_.get(), name.c_str()), &udev_monitor_unref);
	if (!monitor) {
		throw std::bad_alloc();
	}
	return std::make_shared<MonitorImpl>(std::move(monitor));
}

std::shared_ptr<Enumerate> FactoryImpl::make_enumerate()
{
	auto enumerate = make_unref_unique_ptr(udev_enumerate_new(raw_.get()), &udev_enumerate_unref);
	if (!enumerate) {
		throw std::bad_alloc();
	}
	return std::make_shared<EnumerateImpl>(std::move(enumerate));
}

std::shared_ptr<Factory> Factory::make()
{
	auto udev = make_unref_unique_ptr(udev_new(), &udev_unref);
	if (!udev) {
		throw std::bad_alloc();
	}
	return std::make_shared<FactoryImpl>(std::move(udev));
}

}
