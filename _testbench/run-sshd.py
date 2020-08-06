#!/usr/bin/python

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
    s1 = net.addSwitch( 's1', config_file='macbr.click', log_file='./log/br.log')

    info( '*** Creating links\n' )
    l1 = net.addLink( h1, s1 )
    l2 = net.addLink( s1, h2 )

    return net

if __name__ == '__main__':

    setLogLevel( 'info' )
    nw = createTopo()

    info( '*** Starting network\n')
    nw.start()

    info( '*** Starting sshd on hosts\n')
    for h in nw.hosts:
        h.cmd('/usr/sbin/sshd -D -o UseDNS=no -u0 &')

    CLI( nw )

    info( '*** Stopping network' )
    for h in nw.hosts:
        h.cmd('kill %/usr/sbin/sshd')
    nw.stop()
