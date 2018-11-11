Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/xenial64"
  config.vm.network "private_network", ip: "10.254.233.15"
  host = RbConfig::CONFIG['host_os']

  config.vm.provision "shell", path: "bootstrap.sh"
  config.vm.provision "shell", path: "buildzephyr.sh", run: 'always'
  config.vm.provider :virtualbox do |vb|
      if host =~ /linux/
          vb.customize ["modifyvm", :id,  "--cpus", `nproc`.to_i]
      end
    vb.customize ["modifyvm", :id, "--natdnshostresolver1", "on"]
    vb.memory = 2048
  end
end

