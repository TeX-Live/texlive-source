#! /usr/bin/env python3
""" Testcases for zziplib build system """

__copyright__ = "(C) Guido Draheim, all rights reserved"""
__version__ = "0.13.71"

import subprocess
import os.path
import time
import datetime
import unittest
import shutil
import inspect
import types
import logging
import re
from fnmatch import fnmatchcase as fnmatch
from glob import glob
import json
import sys

if sys.version[0] == '3':
    basestring = str
    xrange = range

logg = logging.getLogger("TESTING")
_python = "/usr/bin/python"

SAVETO = "localhost:5000/zziplib"
IMAGES = "localhost:5000/zziplib/image"
CENTOS = "centos:7.7.1908"
UBUNTU = "ubuntu:16.04"
OPENSUSE = "opensuse/leap:15.1"

DOCKER_SOCKET = "/var/run/docker.sock"

def decodes(text):
    if text is None: return None
    if isinstance(text, bytes):
        encoded = sys.getdefaultencoding()
        if encoded in ["ascii"]:
            encoded = "utf-8"
        try: 
            return text.decode(encoded)
        except:
            return text.decode("latin-1")
    return text
def sh____(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    return subprocess.check_call(cmd, shell=shell)
def sx____(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    return subprocess.call(cmd, shell=shell)
def output(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out)
def output2(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out), run.returncode
def output3(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out), decodes(err), run.returncode
def background(cmd, shell=True):
    BackgroundProcess = collections.namedtuple("BackgroundProcess", ["pid", "run", "log" ])
    log = open(os.devnull, "wb")
    run = subprocess.Popen(cmd, shell=shell, stdout=log, stderr=log)
    pid = run.pid
    logg.info("PID %s = %s", pid, cmd)
    return BackgroundProcess(pid, run, log)



def _lines(lines):
    if isinstance(lines, basestring):
        lines = lines.split("\n")
        if len(lines) and lines[-1] == "":
            lines = lines[:-1]
    return lines
def lines(text):
    lines = []
    for line in _lines(text):
        lines.append(line.rstrip())
    return lines
def grep(pattern, lines):
    for line in _lines(lines):
       if re.search(pattern, line.rstrip()):
           yield line.rstrip()
def greps(lines, pattern):
    return list(grep(pattern, lines))

def download(base_url, filename, into):
    if not os.path.isdir(into):
        os.makedirs(into)
    if not os.path.exists(os.path.join(into, filename)):
        sh____("cd {into} && wget {base_url}/{filename}".format(**locals()))
def text_file(filename, content):
    filedir = os.path.dirname(filename)
    if not os.path.isdir(filedir):
        os.makedirs(filedir)
    f = open(filename, "w")
    if content.startswith("\n"):
        x = re.match("(?s)\n( *)", content)
        indent = x.group(1)
        for line in content[1:].split("\n"):
            if line.startswith(indent):
                line = line[len(indent):]
            f.write(line+"\n")
    else:
        f.write(content)
    f.close()
def shell_file(filename, content):
    text_file(filename, content)
    os.chmod(filename, 0o770)
def copy_file(filename, target):
    targetdir = os.path.dirname(target)
    if not os.path.isdir(targetdir):
        os.makedirs(targetdir)
    shutil.copyfile(filename, target)
def copy_tool(filename, target):
    copy_file(filename, target)
    os.chmod(target, 0o750)

def get_caller_name():
    frame = inspect.currentframe().f_back.f_back
    return frame.f_code.co_name
def get_caller_caller_name():
    frame = inspect.currentframe().f_back.f_back.f_back
    return frame.f_code.co_name
def os_path(root, path):
    if not root:
        return path
    if not path:
        return path
    while path.startswith(os.path.sep):
       path = path[1:]
    return os.path.join(root, path)
def docname(path):
    return os.path.splitext(os.path.basename(path))[0]

class ZZiplibBuildTest(unittest.TestCase):
    def caller_testname(self):
        name = get_caller_caller_name()
        x1 = name.find("_")
        if x1 < 0: return name
        x2 = name.find("_", x1+1)
        if x2 < 0: return name
        return name[:x2]
    def testname(self, suffix = None):
        name = self.caller_testname()
        if suffix:
            return name + "_" + suffix
        return name
    def testdir(self, testname = None):
        testname = testname or self.caller_testname()
        newdir = "tmp/tmp."+testname
        if os.path.isdir(newdir):
            shutil.rmtree(newdir)
        os.makedirs(newdir)
        return newdir
    def rm_testdir(self, testname = None):
        testname = testname or self.caller_testname()
        newdir = "tmp/tmp."+testname
        if os.path.isdir(newdir):
            shutil.rmtree(newdir)
        return newdir
    def makedirs(self, path):
        if not os.path.isdir(path):
            os.makedirs(path)
    def user(self):
        import getpass
        getpass.getuser()
    def ip_container(self, name):
        values = output("docker inspect "+name)
        values = json.loads(values)
        if not values or "NetworkSettings" not in values[0]:
            logg.critical(" docker inspect %s => %s ", name, values)
        return values[0]["NetworkSettings"]["IPAddress"]    
    def local_system(self):
        distro, version = "", ""
        if os.path.exists("/etc/os-release"):
            # rhel:7.4 # VERSION="7.4 (Maipo)" ID="rhel" VERSION_ID="7.4"
            # centos:7.3  # VERSION="7 (Core)" ID="centos" VERSION_ID="7"
            # centos:7.4  # VERSION="7 (Core)" ID="centos" VERSION_ID="7"
            # centos:7.7.1908  # VERSION="7 (Core)" ID="centos" VERSION_ID="7"
            # opensuse:42.3 # VERSION="42.3" ID=opensuse VERSION_ID="42.3"
            # opensuse/leap:15.0 # VERSION="15.0" ID="opensuse-leap" VERSION_ID="15.0"
            # ubuntu:16.04 # VERSION="16.04.3 LTS (Xenial Xerus)" ID=ubuntu VERSION_ID="16.04"
            # ubuntu:18.04 # VERSION="18.04.1 LTS (Bionic Beaver)" ID=ubuntu VERSION_ID="18.04"
            for line in open("/etc/os-release"):
                key, value = "", ""
                m = re.match('^([_\\w]+)=([^"].*).*', line.strip())
                if m:
                    key, value = m.group(1), m.group(2)
                m = re.match('^([_\\w]+)="([^"]*)".*', line.strip())
                if m:
                    key, value = m.group(1), m.group(2)
                # logg.debug("%s => '%s' '%s'", line.strip(), key, value)
                if key in ["ID"]:
                    distro = value.replace("-","/")
                if key in ["VERSION_ID"]:
                    version = value
        if os.path.exists("/etc/redhat-release"):
            for line in open("/etc/redhat-release"):
                m = re.search("release (\\d+[.]\\d+).*", line)
                if m:
                    distro = "rhel"
                    version = m.group(1)
        if os.path.exists("/etc/centos-release"):
            # CentOS Linux release 7.5.1804 (Core)
            for line in open("/etc/centos-release"):
                m = re.search("release (\\d+[.]\\d+).*", line)
                if m:
                    distro = "centos"
                    version = m.group(1)
        logg.info(":: local_system %s:%s", distro, version)
        if distro and version:
            return "%s:%s" % (distro, version)
        return ""
    def with_local_ubuntu_mirror(self, ver = None):
        """ detects a local ubuntu mirror or starts a local
            docker container with a ubunut repo mirror. It
            will return the extra_hosts setting to start
            other docker containers"""
        rmi = "localhost:5000/mirror-packages"
        rep = "ubuntu-repo"
        ver = ver or UBUNTU.split(":")[1]
        universe = "ubuntu-repo/universe"
        ok = self.with_local(rmi, universe, ver, "archive.ubuntu.com", "security.ubuntu.com")
        if ok: return ok
        return self.with_local(rmi, rep, ver, "archive.ubuntu.com", "security.ubuntu.com")
    def with_local_centos_mirror(self, ver = None):
        """ detects a local centos mirror or starts a local
            docker container with a centos repo mirror. It
            will return the setting for extrahosts"""
        rmi = "localhost:5000/mirror-packages"
        rep = "centos-repo"
        ver = ver or CENTOS.split(":")[1]
        return self.with_local(rmi, rep, ver, "mirrorlist.centos.org")
    def with_local_opensuse_mirror(self, ver = None):
        """ detects a local opensuse mirror or starts a local
            docker container with a centos repo mirror. It
            will return the extra_hosts setting to start
            other docker containers"""
        rmi = "localhost:5000/mirror-packages"
        rep = "opensuse-repo"
        ver = ver or OPENSUSE.split(":")[1]
        return self.with_local(rmi, rep, ver, "download.opensuse.org")
    def with_local(self, rmi, rep, ver, *hosts):
        image = "{rmi}/{rep}:{ver}".format(**locals())
        container = "{rep}-{ver}".format(**locals()).replace("/","-")
        out, err, ok = output3("docker inspect {image}".format(**locals()))
        image_found = json.loads(out)
        if not image_found:
           return {}
        out, err, ok = output3("docker inspect {container}".format(**locals()))
        container_found = json.loads(out)
        if container_found:
            container_status = container_found[0]["State"]["Status"]
            logg.info("::: %s -> %s", container, container_status)
            latest_image_id = image_found[0]["Id"]
            container_image_id = container_found[0]["Image"]
            if latest_image_id != container_image_id or container_status not in ["running"]:
                cmd = "docker rm --force {container}"
                sx____(cmd.format(**locals()))
                container_found = []
        if not container_found:
            cmd = "docker run --rm=true --detach --name {container} {image}"
            sh____(cmd.format(**locals()))
        ip_a = self.ip_container(container)
        logg.info("::: %s => %s", container, ip_a)
        return dict(zip(hosts, [ ip_a ] * len(hosts)))
    def with_local_mirror(self, image):
        """ attach local centos-repo / opensuse-repo to docker-start enviroment.
            Effectivly when it is required to 'docker start centos:x.y' then do
            'docker start centos-repo:x.y' before and extend the original to 
            'docker start --add-host mirror...:centos-repo centos:x.y'. """
        hosts = {}
        if image.startswith("centos:"):
            version = image[len("centos:"):]
            hosts = self.with_local_centos_mirror(version)
        if image.startswith("opensuse/leap:"):
            version = image[len("opensuse/leap:"):]
            hosts = self.with_local_opensuse_mirror(version)
        if image.startswith("opensuse:"):
            version = image[len("opensuse:"):]
            hosts = self.with_local_opensuse_mirror(version)
        if image.startswith("ubuntu:"):
            version = image[len("ubuntu:"):]
            hosts = self.with_local_ubuntu_mirror(version)
        return hosts
    def add_hosts(self, hosts):
        return " ".join(["--add-host %s:%s" % (host, ip_a) for host, ip_a in hosts.items() ])
        # for host, ip_a in mapping.items():
        #    yield "--add-host {host}:{ip_a}"
    def local_image(self, image):
        """ attach local centos-repo / opensuse-repo to docker-start enviroment.
            Effectivly when it is required to 'docker start centos:x.y' then do
            'docker start centos-repo:x.y' before and extend the original to 
            'docker start --add-host mirror...:centos-repo centos:x.y'. """
        if os.environ.get("NONLOCAL",""):
            return image
        hosts =  self.with_local_mirror(image)
        if hosts:
            add_hosts = self.add_hosts(hosts)
            logg.debug("%s %s", add_hosts, image)
            return "{add_hosts} {image}".format(**locals())
        return image
    def local_addhosts(self, dockerfile):
        image = ""
        for line in open(dockerfile):
            m = re.match('[Ff][Rr][Oo][Mm] *"([^"]*)"', line)
            if m: 
                image = m.group(1)
                break
            m = re.match("[Ff][Rr][Oo][Mm] *(\w[^ ]*)", line)
            if m: 
                image = m.group(1).strip()
                break
        logg.debug("--\n-- '%s' FROM '%s'", dockerfile, image)
        if image:
            hosts = self.with_local_mirror(image)
            return self.add_hosts(hosts)
        return ""
    def drop_container(self, name):
        cmd = "docker rm --force {name}"
        sx____(cmd.format(**locals()))
    def drop_centos(self):
        self.drop_container("centos")
    def drop_ubuntu(self):
        self.drop_container("ubuntu")
    def drop_opensuse(self):
        self.drop_container("opensuse")
    def make_opensuse(self):
        self.make_container("opensuse", OPENSUSE)
    def make_ubuntu(self):
        self.make_container("ubuntu", UBUNTU)
    def make_centos(self):
        self.make_container("centos", CENTOS)
    def make_container(self, name, image):
        self.drop_container(name)
        local_image = self.local_image(image)
        cmd = "docker run --detach --name {name} {local_image} sleep 1000"
        sh____(cmd.format(**locals()))
        print("                 # " + local_image)
        print("  docker exec -it "+name+" bash")
    #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    #
    def test_100(self):
        logg.info("\n  CENTOS = '%s'", CENTOS)
        self.with_local_centos_mirror()
    def test_201_centos7_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_202_centos8_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-am-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_211_centos7_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_212_centos8_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_221_ubuntu16_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/x86_64-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_222_ubuntu18_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu18-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/x86_64-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_225_ubuntu16_32bit_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-32bit.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/i386-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_231_opensuse15_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/opensuse15-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    @unittest.expectedFailure
    def test_251_windows_static_x64_dockerfile(self):
        logg.warning("     windows-static-x64 compiles fine but segfaults on linking an .exe")
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/windows-static-x64.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    @unittest.expectedFailure
    def test_252_windows_shared_x64_dockerfile(self):
        logg.warning("     windows-shared-x64 compiles fine but segfaults on linking an .exe")
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/windows-shared-x64.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_301_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_302_centos8_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-am-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_311_centos7_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_312_centos8_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_321_ubuntu18_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_331_opensuse15_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/opensuse15-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_411_centos7_sdl2_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /new/usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /new/usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /new/usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /new/usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_412_centos8_sdl2_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /new/usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /new/usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /new/usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /new/usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_421_ubuntu16_azure_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-azure.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_install=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} find src -name *.xml"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_701_centos7_am_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_build=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/zzip/types.h'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/share/doc/zziplib'"
        sh____(cmd.format(**locals()))    
        cmd = "docker exec {testname} bash -c 'test -f /usr/local/share/man/man3/zzip_opendir.3'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_711_centos7_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build" # "build --build-arg=no_build=true"
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/zzip/types.h'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/share/doc/zziplib'"
        sh____(cmd.format(**locals()))    
        cmd = "docker exec {testname} bash -c 'test -f /usr/local/share/man/man3/zzip_opendir.3'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9211_centos7_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-build.dockerfile"
        dockerfile2="testbuilds/centos7-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9212_centos7_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-am-build.dockerfile"
        dockerfile2="testbuilds/centos8-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9311_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-sdl2.dockerfile"
        dockerfile2="testbuilds/centos7-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9312_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-am-sdl2.dockerfile"
        dockerfile2="testbuilds/centos8-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9411_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-sdl2.dockerfile"
        dockerfile2="testbuilds/centos7-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /new/usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        DIRS="etc lib libexec sbin games src share/info share/applications share/man/mann"
        for i in xrange(1,10):
           DIRS+=" share/man/man%i share/man/man%ix" % (i,i)
        cmd = "docker exec {testname1} bash -c 'cd /new/local && (for u in {DIRS}; do mkdir -pv $u; done)'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9412_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-sdl2.dockerfile"
        dockerfile2="testbuilds/centos8-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /new/usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        DIRS="etc lib libexec sbin games src share/info share/applications share/man/mann"
        for i in xrange(1,10):
           DIRS+=" share/man/man%i share/man/man%ix" % (i,i)
        cmd = "docker exec {testname1} bash -c 'cd /new/local && (for u in {DIRS}; do mkdir -pv $u; done)'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_9711_centos7_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        testname1=self.testname() + "_1"
        testname2=self.testname() + "_2"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-docs.dockerfile"
        dockerfile2="testbuilds/centos7-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && mv man/html .'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && rm -rf man'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && mv html man'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference --brief /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()


if __name__ == "__main__":
    from optparse import OptionParser
    _o = OptionParser("%prog [options] test*",
       epilog=__doc__.strip().split("\n")[0])
    _o.add_option("-v","--verbose", action="count", default=0,
       help="increase logging level [%default]")
    _o.add_option("-p","--python", metavar="EXE", default=_python,
       help="use another python execution engine [%default]")
    _o.add_option("-l","--logfile", metavar="FILE", default="",
       help="additionally save the output log to a file [%default]")
    _o.add_option("--xmlresults", metavar="FILE", default=None,
       help="capture results as a junit xml file [%default]")
    opt, args = _o.parse_args()
    logging.basicConfig(level = logging.WARNING - opt.verbose * 5)
    #
    _python = opt.python
    #
    logfile = None
    if opt.logfile:
        if os.path.exists(opt.logfile):
           os.remove(opt.logfile)
        logfile = logging.FileHandler(opt.logfile)
        logfile.setFormatter(logging.Formatter("%(levelname)s:%(relativeCreated)d:%(message)s"))
        logging.getLogger().addHandler(logfile)
        logg.info("log diverted to %s", opt.logfile)
    xmlresults = None
    if opt.xmlresults:
        if os.path.exists(opt.xmlresults):
           os.remove(opt.xmlresults)
        xmlresults = open(opt.xmlresults, "w")
        logg.info("xml results into %s", opt.xmlresults)
    #
    # unittest.main()
    suite = unittest.TestSuite()
    if not args: args = [ "test_*" ]
    for arg in args:
        for classname in sorted(globals()):
            if not classname.endswith("Test"):
                continue
            testclass = globals()[classname]
            for method in sorted(dir(testclass)):
                if "*" not in arg: arg += "*"
                if arg.startswith("_"): arg = arg[1:]
                if fnmatch(method, arg):
                    suite.addTest(testclass(method))
    # select runner
    if not logfile:
        if xmlresults:
            import xmlrunner
            Runner = xmlrunner.XMLTestRunner
            Runner(xmlresults).run(suite)
        else:
            Runner = unittest.TextTestRunner
            Runner(verbosity=opt.verbose).run(suite)
    else:
        Runner = unittest.TextTestRunner
        if xmlresults:
            import xmlrunner
            Runner = xmlrunner.XMLTestRunner
        Runner(logfile.stream, verbosity=opt.verbose).run(suite)
