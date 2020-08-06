#!/usr/bin/python

import sys

# print(sys.path)

from mininet.net import Mininet
from mininet.node import Controller, OVSSwitch, Switch, Node
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.util import errFail, waitListening

class ClickUserSwitch(Switch):
    binary = 'click'

    def __init__(self, name, config_file, log_file='/dev/null', inNamespace=True, parameters={}, **params):
        Switch.__init__( self, name, inNamespace=inNamespace, **params )
        self.config_file = config_file
        self.log_file = log_file
        self.params = parameters

    def start(self, controllers):
        cmd = [ClickUserSwitch.binary]
        for name in self.params:
            cmd.append(name + '=' + self.params[name])
        cmd.append(self.config_file)
        if self.log_file:
            cmd.append('> "%s" 2>&1' % self.log_file)
        self.cmd(" ".join(cmd) + " &")

    def stop(self):
        self.cmd('kill %click')
        pass

def createTopo():

    net = Mininet( controller=Controller, switch=ClickUserSwitch, link=TCLink )

    info( '*** Adding controller\n' )
    net.addController( 'c0' )

    info( '*** Adding hosts\n' )
    h1 = net.addHost( 'h1')
    h2 = net.addHost( 'h2')

    info( '*** Adding switch\n' )
    s1 = net.addSwitch( 's1', config_file='switch.click', log_file='./log/s1.log', parameters=dict(HOST='s1-eth1', NETWORK='s1-eth2') )
    s2 = net.addSwitch( 's2', config_file='switch.click', log_file='./log/s2.log', parameters=dict(HOST='s2-eth2', NETWORK='s2-eth1') )
    s3 = net.addSwitch( 's3', config_file='macbr.click', log_file='./log/br.log')

    info( '*** Creating links\n' )
    l1 = net.addLink( h1, s1 )
    l2 = net.addLink( s1, s3 )
    l3 = net.addLink( s3, s2 )
    l4 = net.addLink( h2, s2 )
    l2.intf1.ifconfig("mtu", "50000")
    l2.intf2.ifconfig("mtu", "50000")
    l3.intf1.ifconfig("mtu", "50000")
    l3.intf2.ifconfig("mtu", "50000")

    return net

if __name__ == '__main__':

    setLogLevel( 'info' )
    nw = createTopo()

    info( '*** Starting network\n')
    nw.start()

    info( '*** Starting sshd on hosts\n')
    for h in nw.hosts:
        h.cmd('/usr/sbin/sshd -D -o UseDNS=no -u0 &')
    # for h in nw.hosts:
    #     waitListening(client=nw['h1'], server=h, port=22, timeout=2)

    CLI( nw )

    info( '*** Stopping network' )
    for h in nw.hosts:
        h.cmd('kill %/usr/sbin/sshd')
    nw.stop()
