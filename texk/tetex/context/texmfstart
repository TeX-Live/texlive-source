#!/usr/bin/env ruby

# program   : texmfstart
# copyright : PRAGMA Advanced Document Engineering
# version   : 1.9.0 - 2003/2006
# author    : Hans Hagen
#
# project   : ConTeXt / eXaMpLe
# info      : j.hagen@xs4all.nl
# www       : www.pragma-pod.com / www.pragma-ade.com

# no special requirements, i.e. no exa modules/classes used

# texmfstart [switches] filename [optional arguments]
#
# ruby2exe texmfstart --help -> avoids stub test
#
# Of couse I can make this into a nice class, which i'll undoubtely will
# do when I feel the need. In that case it will be part of a bigger game.

# turning this into a service would be nice, so some day ...

# --locate        => provides location
# --exec          => exec instead of system
# --iftouched=a,b => only if timestamp a<>b
# --ifchanged=a,b => only if checksum changed
#
# file: path: bin:

# texmfstart --exec bin:scite *.tex

# we don't depend on other libs

$: << File.expand_path(File.dirname($0)) ; $: << File.join($:.last,'lib') ; $:.uniq!

require "rbconfig"
require "md5"

# kpse_merge_done: require 'base/kpseremote'
# kpse_merge_done: require 'base/kpsedirect'
# kpse_merge_done: require 'base/kpsefast'
# kpse_merge_done: require 'base/merge'

# kpse_merge_start

# kpse_merge_file: 'C:/data/develop/context/ruby/base/kpseremote.rb'

# kpse_merge_done: require 'base/kpsefast'

case ENV['KPSEMETHOD']
    when /soap/o then # kpse_merge_done: require 'base/kpse/soap'
    when /drb/o  then # kpse_merge_done: require 'base/kpse/drb'
    else              # kpse_merge_done: require 'base/kpse/drb'
end

class KpseRemote

    @@port   = ENV['KPSEPORT']   || 7000
    @@method = ENV['KPSEMETHOD'] || 'drb'

    def KpseRemote::available?
        @@method && @@port
    end

    def KpseRemote::start_server(port=nil)
        kpse = KpseServer.new(port || @@port)
        kpse.start
    end

    def KpseRemote::start_client(port=nil) # keeps object in server
        kpseclient = KpseClient.new(port || @@port)
        kpseclient.start
        kpse = kpseclient.object
        tree = kpse.choose(KpseUtil::identify, KpseUtil::environment)
        [kpse, tree]
    end

    def KpseRemote::fetch(port=nil) # no need for defining methods but slower, send whole object
        kpseclient = KpseClient.new(port || @@port)
        kpseclient.start
        kpseclient.object.fetch(KpseUtil::identify, KpseUtil::environment) rescue nil
    end

    def initialize(port=nil)
        if KpseRemote::available? then
            begin
                @kpse, @tree = KpseRemote::start_client(port)
            rescue
                @kpse, @tree = nil, nil
            end
        else
            @kpse, @tree = nil, nil
        end
    end

    def progname=(value)
        @kpse.set(@tree,'progname',value)
    end
    def format=(value)
        @kpse.set(@tree,'format',value)
    end
    def engine=(value)
        @kpse.set(@tree,'engine',value)
    end

    def progname
        @kpse.get(@tree,'progname')
    end
    def format
        @kpse.get(@tree,'format')
    end
    def engine
        @kpse.get(@tree,'engine')
    end

    def load
        @kpse.load(KpseUtil::identify, KpseUtil::environment)
    end
    def okay?
        @kpse && @tree
    end
    def set(key,value)
        @kpse.set(@tree,key,value)
    end
    def load_cnf
        @kpse.load_cnf(@tree)
    end
    def load_lsr
        @kpse.load_lsr(@tree)
    end
    def expand_variables
        @kpse.expand_variables(@tree)
    end
    def expand_braces(str)
        clean_name(@kpse.expand_braces(@tree,str))
    end
    def expand_path(str)
        clean_name(@kpse.expand_path(@tree,str))
    end
    def expand_var(str)
        clean_name(@kpse.expand_var(@tree,str))
    end
    def show_path(str)
        clean_name(@kpse.show_path(@tree,str))
    end
    def var_value(str)
        clean_name(@kpse.var_value(@tree,str))
    end
    def find_file(filename)
        clean_name(@kpse.find_file(@tree,filename))
    end
    def find_files(filename,first=false)
        # dodo: each filename
        @kpse.find_files(@tree,filename,first)
    end

    private

    def clean_name(str)
        str.gsub(/\\/,'/')
    end

end


# kpse_merge_file: 'C:/data/develop/context/ruby/base/kpsefast.rb'

# module    : base/kpsefast
# copyright : PRAGMA Advanced Document Engineering
# version   : 2005
# author    : Hans Hagen
#
# project   : ConTeXt / eXaMpLe
# concept   : Hans Hagen
# info      : j.hagen@xs4all.nl

# todo: multiple cnf files
#

class String

    def split_path
        if self =~ /\;/o || self =~ /^[a-z]\:/io then
            self.split(";")
        else
            self.split(":")
        end
    end

end

class Array

    def join_path
        self.join(File::PATH_SEPARATOR)
    end

end

class File

    def File.locate_file(path,name)
        begin
            files = Dir.entries(path)
            if files.include?(name) then
                fullname = File.join(path,name)
                return fullname if FileTest.file?(fullname)
            end
            files.each do |p|
                fullname = File.join(path,p)
                if p != '.' and p != '..' and FileTest.directory?(fullname) and result = locate_file(fullname,name) then
                    return result
                end
            end
        rescue
            # bad path
        end
        return nil
    end

    def File.glob_file(pattern)
        return Dir.glob(pattern).first
    end

end

module KpseUtil

    # to be adapted, see loading cnf file

    @@texmftrees = ['texmf-local','texmf.local','../..','texmf'] # '../..' is for gwtex
    @@texmfcnf   = 'texmf.cnf'

    def KpseUtil::identify
        # we mainly need to identify the local tex stuff and wse assume that
        # the texmfcnf variable is set; otherwise we need to expand the
        # TEXMF variable and that takes time since it may involve more
        ownpath = File.expand_path($0)
        if ownpath.gsub!(/texmf.*?$/o, '') then
            ENV['SELFAUTOPARENT'] = ownpath
        else
            ENV['SELFAUTOPARENT'] = '.' # fall back
            # may be too tricky:
            #
            # (ENV['PATH'] ||'').split_path.each do |p|
                # if p.gsub!(/texmf.*?$/o, '') then
                    # ENV['SELFAUTOPARENT'] = p
                    # break
                # end
            # end
        end
        filenames = Array.new
        if ENV['TEXMFCNF'] && ! ENV['TEXMFCNF'].empty? then
            ENV['TEXMFCNF'].to_s.split_path.each do |path|
                filenames << File.join(path,@@texmfcnf)
            end
        elsif ENV['SELFAUTOPARENT'] == '.' then
            filenames << File.join('.',@@texmfcnf)
        else
            @@texmftrees.each do |tree|
                filenames << File.join(ENV['SELFAUTOPARENT'],tree,'web2c',@@texmfcnf)
            end
        end
        loop do
            busy = false
            filenames.collect! do |f|
                f.gsub(/\$([a-zA-Z0-9\_\-]+)/o) do
                    if (! ENV[$1]) || (ENV[$1] == $1) then
                        "$#{$1}"
                    else
                        busy = true
                        ENV[$1]
                    end
                end
            end
            break unless busy
        end
        filenames.delete_if do |f|
            ! FileTest.file?(f)
        end
        return filenames
    end

    def KpseUtil::environment
        Hash.new.merge(ENV)
    end

end

class KpseFast

    # formats are an incredible inconsistent mess

    @@suffixes  = Hash.new
    @@formats   = Hash.new
    @@suffixmap = Hash.new

    @@texmfcnf  = 'texmf.cnf'

    @@suffixes['gf']                       = ['.<resolution>gf'] # todo
    @@suffixes['pk']                       = ['.<resolution>pk'] # todo
    @@suffixes['tfm']                      = ['.tfm']
    @@suffixes['afm']                      = ['.afm']
    @@suffixes['base']                     = ['.base']
    @@suffixes['bib']                      = ['.bib']
    @@suffixes['bst']                      = ['.bst']
    @@suffixes['cnf']                      = ['.cnf']
    @@suffixes['ls-R']                     = ['ls-R', 'ls-r']
    @@suffixes['fmt']                      = ['.fmt', '.efmt', '.efm', '.ofmt', '.ofm', '.oft', '.eofmt', '.eoft', '.eof', '.pfmt', '.pfm', '.epfmt', '.epf', '.xpfmt', '.xpf', '.afmt', '.afm']
    @@suffixes['map']                      = ['.map']
    @@suffixes['mem']                      = ['.mem']
    @@suffixes['mf']                       = ['.mf']
    @@suffixes['mfpool']                   = ['.pool']
    @@suffixes['mft']                      = ['.mft']
    @@suffixes['mp']                       = ['.mp']
    @@suffixes['mppool']                   = ['.pool']
    @@suffixes['ocp']                      = ['.ocp']
    @@suffixes['ofm']                      = ['.ofm', '.tfm']
    @@suffixes['opl']                      = ['.opl']
    @@suffixes['otp']                      = ['.otp']
    @@suffixes['ovf']                      = ['.ovf']
    @@suffixes['ovp']                      = ['.ovp']
    @@suffixes['graphic/figure']           = ['.eps', '.epsi']
    @@suffixes['tex']                      = ['.tex']
    @@suffixes['texpool']                  = ['.pool']
    @@suffixes['PostScript header']        = ['.pro']
    @@suffixes['type1 fonts']              = ['.pfa', '.pfb']
    @@suffixes['vf']                       = ['.vf']
    @@suffixes['ist']                      = ['.ist']
    @@suffixes['truetype fonts']           = ['.ttf', '.ttc']
    @@suffixes['web']                      = ['.web', '.ch']
    @@suffixes['cweb']                     = ['.w', '.web', '.ch']
    @@suffixes['enc files']                = ['.enc']
    @@suffixes['cmap files']               = ['.cmap']
    @@suffixes['subfont definition files'] = ['.sfd']
    @@suffixes['lig files']                = ['.lig']
    @@suffixes['bitmap font']              = []
    @@suffixes['MetaPost support']         = []
    @@suffixes['TeX system documentation'] = []
    @@suffixes['TeX system sources']       = []
    @@suffixes['Troff fonts']              = []
    @@suffixes['dvips config']             = []
    @@suffixes['type42 fonts']             = []
    @@suffixes['web2c files']              = []
    @@suffixes['other text files']         = []
    @@suffixes['other binary files']       = []
    @@suffixes['misc fonts']               = []
    @@suffixes['opentype fonts']           = []
    @@suffixes['pdftex config']            = []
    @@suffixes['texmfscripts']             = []

    # replacements

    @@suffixes['fmt']                      = ['.fmt']
    @@suffixes['type1 fonts']              = ['.pfa', '.pfb', '.pfm']
    @@suffixes['tex']                      = ['.tex', '.xml']
    @@suffixes['texmfscripts']             = ['rb','lua','py','pl']

    @@suffixes.keys.each do |k| @@suffixes[k].each do |s| @@suffixmap[s] = k end end

    # TTF2TFMINPUTS
    # MISCFONTS
    # TEXCONFIG
    # DVIPDFMINPUTS
    # OTFFONTS

    @@formats['gf']                       = ''
    @@formats['pk']                       = ''
    @@formats['tfm']                      = 'TFMFONTS'
    @@formats['afm']                      = 'AFMFONTS'
    @@formats['base']                     = 'MFBASES'
    @@formats['bib']                      = ''
    @@formats['bst']                      = ''
    @@formats['cnf']                      = ''
    @@formats['ls-R']                     = ''
    @@formats['fmt']                      = 'TEXFORMATS'
    @@formats['map']                      = 'TEXFONTMAPS'
    @@formats['mem']                      = 'MPMEMS'
    @@formats['mf']                       = 'MFINPUTS'
    @@formats['mfpool']                   = 'MFPOOL'
    @@formats['mft']                      = ''
    @@formats['mp']                       = 'MPINPUTS'
    @@formats['mppool']                   = 'MPPOOL'
    @@formats['ocp']                      = 'OCPINPUTS'
    @@formats['ofm']                      = 'OFMFONTS'
    @@formats['opl']                      = 'OPLFONTS'
    @@formats['otp']                      = 'OTPINPUTS'
    @@formats['ovf']                      = 'OVFFONTS'
    @@formats['ovp']                      = 'OVPFONTS'
    @@formats['graphic/figure']           = ''
    @@formats['tex']                      = 'TEXINPUTS'
    @@formats['texpool']                  = 'TEXPOOL'
    @@formats['PostScript header']        = 'TEXPSHEADERS'
    @@formats['type1 fonts']              = 'T1FONTS'
    @@formats['vf']                       = 'VFFONTS'
    @@formats['ist']                      = ''
    @@formats['truetype fonts']           = 'TTFONTS'
    @@formats['web']                      = ''
    @@formats['cweb']                     = ''
    @@formats['enc files']                = 'ENCFONTS'
    @@formats['cmap files']               = 'CMAPFONTS'
    @@formats['subfont definition files'] = 'SFDFONTS'
    @@formats['lig files']                = 'LIGFONTS'
    @@formats['bitmap font']              = ''
    @@formats['MetaPost support']         = ''
    @@formats['TeX system documentation'] = ''
    @@formats['TeX system sources']       = ''
    @@formats['Troff fonts']              = ''
    @@formats['dvips config']             = ''
    @@formats['type42 fonts']             = 'T42FONTS'
    @@formats['web2c files']              = 'WEB2C'
    @@formats['other text files']         = ''
    @@formats['other binary files']       = ''
    @@formats['misc fonts']               = ''
    @@formats['opentype fonts']           = 'OPENTYPEFONTS'
    @@formats['pdftex config']            = 'PDFTEXCONFIG'
    @@formats['texmfscripts']             = 'TEXMFSCRIPTS'

    attr_accessor :progname, :engine, :format, :rootpath, :treepath,
        :verbose, :remember, :scandisk, :diskcache, :renewcache

    @@cacheversion = '1'

    def initialize
        @rootpath    = ''
        @treepath    = ''
        @progname    = 'kpsewhich'
        @engine      = 'pdftex'
        @variables   = Hash.new
        @expansions  = Hash.new
        @files       = Hash.new
        @found       = Hash.new
        @kpsevars    = Hash.new
        @lsrfiles    = Array.new
        @cnffiles    = Array.new
        @verbose     = true
        @remember    = true
        @scandisk    = true
        @diskcache   = true
        @renewcache  = false
        @isolate     = false

        @diskcache   = false
        @cachepath   = nil
        @cachefile   = 'tmftools.log'

        @environment = ENV
    end

    def set(key,value)
        case key
            when 'progname' then @progname = value
            when 'engine'   then @engine   = value
            when 'format'   then @format   = value
        end
    end

    def push_environment(env)
        @environment = env
    end

    # {$SELFAUTOLOC,$SELFAUTODIR,$SELFAUTOPARENT}{,{/share,}/texmf{-local,}/web2c}
    #
    # $SELFAUTOLOC    : /usr/tex/bin/platform
    # $SELFAUTODIR    : /usr/tex/bin
    # $SELFAUTOPARENT : /usr/tex
    #
    # since we live in scriptpath we need a slightly different method

    def load_cnf(filenames=nil)
        unless filenames then
            ownpath = File.expand_path($0)
            if ownpath.gsub!(/texmf.*?$/o, '') then
                @environment['SELFAUTOPARENT'] = ownpath
            else
                @environment['SELFAUTOPARENT'] = '.'
            end
            unless @treepath.empty? then
                unless @rootpath.empty? then
                    @treepath = @treepath.split(',').collect do |p| File.join(@rootpath,p) end.join(',')
                end
                @environment['TEXMF'] = @treepath
                # only the first one
                @environment['TEXMFCNF'] = File.join(@treepath.split(',').first,'texmf/web2c')
            end
            unless @rootpath.empty? then
                @environment['TEXMFCNF'] = File.join(@rootpath,'texmf/web2c')
                @environment['SELFAUTOPARENT'] = @rootpath
                @isolate = true
            end
            filenames = Array.new
            if @environment['TEXMFCNF'] and not @environment['TEXMFCNF'].empty? then
                @environment['TEXMFCNF'].to_s.split_path.each do |path|
                    filenames << File.join(path,@@texmfcnf)
                end
            elsif @environment['SELFAUTOPARENT'] == '.' then
                filenames << File.join('.',@@texmfcnf)
            else
                ['texmf-local','texmf'].each do |tree|
                    filenames << File.join(@environment['SELFAUTOPARENT'],tree,'web2c',@@texmfcnf)
                end
            end
        end
        # <root>/texmf/web2c/texmf.cnf
        filenames = _expanded_path_(filenames)
        @rootpath = filenames.first
        3.times do
            @rootpath = File.dirname(@rootpath)
        end
        filenames.collect! do |f|
            f.gsub("\\", '/')
        end
        filenames.each do |fname|
            if FileTest.file?(fname) and f = File.open(fname) then
                @cnffiles << fname
                while line = f.gets do
                    loop do
                        # concatenate lines ending with \
                        break unless line.sub!(/\\\s*$/o) do
                            f.gets || ''
                        end
                    end
                    case line
                        when /^[\%\#]/o then
                            # comment
                        when /^\s*(.*?)\s*\=\s*(.*?)\s*$/o then
                            key, value = $1, $2
                            unless @variables.key?(key) then
                                value.sub!(/\%.*$/,'')
                                value.sub!(/\~/, "$HOME")
                                @variables[key] = value
                            end
                            @kpsevars[key] = true
                    end
                end
                f.close
            end
        end
    end

    def load_lsr
        @lsrfiles = []
        simplified_list(expansion('TEXMF')).each do |p|
            ['ls-R','ls-r'].each do |f|
                filename = File.join(p,f)
                if FileTest.file?(filename) then
                    @lsrfiles << [filename,File.size(filename)]
                    break
                end
            end
        end
        @files = Hash.new
        if @diskcache then
            ['HOME','TEMP','TMP','TMPDIR'].each do |key|
                if @environment[key] then
                    if FileTest.directory?(@environment[key]) then
                        @cachepath = @environment[key]
                        @cachefile = [@rootpath.gsub(/[^A-Z0-9]/io, '-').gsub(/\-+/,'-'),File.basename(@cachefile)].join('-')
                        break
                    end
                end
            end
            if @cachepath and not @renewcache and FileTest.file?(File.join(@cachepath,@cachefile)) then
                begin
                    if f = File.open(File.join(@cachepath,@cachefile)) then
                        cacheversion = Marshal.load(f)
                        if cacheversion == @@cacheversion then
                            lsrfiles = Marshal.load(f)
                            if lsrfiles == @lsrfiles then
                                @files = Marshal.load(f)
                            end
                        end
                        f.close
                    end
                rescue
                   @files = Hash.new
               end
            end
        end
        return if @files.size > 0
        @lsrfiles.each do |filedata|
            filename, filesize = filedata
            filepath = File.dirname(filename)
            begin
                path = '.'
                data = IO.readlines(filename)
                if data[0].chomp =~ /% ls\-R \-\- filename database for kpathsea\; do not change this line\./io then
                    data.each do |line|
                        case line
                            when /^[a-zA-Z0-9]/o then
                                line.chomp!
                                if @files[line] then
                                    @files[line] << path
                                else
                                    @files[line] = [path]
                                end
                            when /^\.\/(.*?)\:$/o then
                                path = File.join(filepath,$1)
                        end
                    end
                end
            rescue
                # sorry
            end
        end
        if @diskcache and @cachepath and f = File.open(File.join(@cachepath,@cachefile),'wb') then
            f << Marshal.dump(@@cacheversion)
            f << Marshal.dump(@lsrfiles)
            f << Marshal.dump(@files)
            f.close
        end
    end

    def expand_variables
        @expansions = Hash.new
        if @isolate then
            @variables['TEXMFCNF'] = @environment['TEXMFCNF'].dup
            @variables['SELFAUTOPARENT'] = @environment['SELFAUTOPARENT'].dup
        else
            @environment.keys.each do |e|
                if e =~ /^([a-zA-Z]+)\_(.*)\s*$/o then
                    @expansions["#{$1}.#{$2}"] = (@environment[e] ||'').dup
                else
                    @expansions[e] = (@environment[e] ||'').dup
                end
            end
        end
        @variables.keys.each do |k|
            @expansions[k] = @variables[k].dup unless @expansions[k]
        end
        loop do
            busy = false
            @expansions.keys.each do |k|
                @expansions[k].gsub!(/\$([a-zA-Z0-9\_\-]*)/o) do
                    busy = true
                    @expansions[$1] || ''
                end
                @expansions[k].gsub!(/\$\{([a-zA-Z0-9\_\-]*)\}/o) do
                    busy = true
                    @expansions[$1] || ''
                end
            end
            break unless busy
        end
        @expansions.keys.each do |k|
            @expansions[k] = @expansions[k].gsub("\\", '/')
        end
    end

    def variable(name='')
        (name and not name.empty? and @variables[name.sub('$','')]) or  ''
    end

    def expansion(name='')
        (name and not name.empty? and @expansions[name.sub('$','')]) or ''
    end

    def variable?(name='')
        name and not name.empty? and @variables.key?(name.sub('$',''))
    end

    def expansion?(name='')
        name and not name.empty? and @expansions.key?(name.sub('$',''))
    end

    def simplified_list(str)
        lst = str.gsub(/^\{/o,'').gsub(/\}$/o,'').split(",")
        lst.collect do |l|
            l.sub(/^[\!]*/,'').sub(/[\/\\]*$/o,'')
        end
    end

    def original_variable(variable)
        if variable?("#{@progname}.#{variable}") then
            variable("#{@progname}.#{variable}")
        elsif variable?(variable) then
            variable(variable)
        else
            ''
        end
    end

    def expanded_variable(variable)
        if expansion?("#{variable}.#{@progname}") then
            expansion("#{variable}.#{@progname}")
        elsif expansion?(variable) then
            expansion(variable)
        else
            ''
        end
    end

    def original_path(filename='')
        _expanded_path_(original_variable(var_of_format_or_suffix(filename)).split(";"))
    end

    def expanded_path(filename='')
        _expanded_path_(expanded_variable(var_of_format_or_suffix(filename)).split(";"))
    end

    def _expanded_path_(pathlist)
        i, n = 0, 0
        pathlist.collect! do |mainpath|
            mainpath.gsub(/([\{\}])/o) do
                if $1 == "{" then
                    i += 1 ; n = i if i > n ; "<#{i}>"
                else
                    i -= 1 ; "</#{i+1}>"
                end
            end
        end
        n.times do |i|
            loop do
                more = false
                newlist = []
                pathlist.each do |path|
                    unless path.sub!(/^(.*?)<(#{n-i})>(.*?)<\/\2>(.*?)$/) do
                        pre, mid, post = $1, $3, $4
                        mid.gsub!(/\,$/,',.')
                        mid.split(',').each do |m|
                            more = true
                            if m == '.' then
                                newlist << "#{pre}#{post}"
                            else
                                newlist << "#{pre}#{m}#{post}"
                            end
                        end
                    end then
                        newlist << path
                    end
                end
                if more then
                    pathlist = [newlist].flatten # copy -)
                else
                    break
                end
            end
        end
        pathlist = pathlist.uniq.collect do |path|
            p = path
            # p.gsub(/^\/+/o) do '' end
            # p.gsub!(/(.)\/\/(.)/o) do "#{$1}/#{$2}" end
            # p.gsub!(/\/\/+$/o) do '//' end
            p.gsub!(/\/\/+/o) do '//' end
            p
        end
        pathlist
    end

    # todo: ignore case

    def var_of_format(str)
        @@formats[str] || ''
    end

    def var_of_suffix(str) # includes .
        if @@suffixmap.key?(str) then @@formats[@@suffixmap[str]] else '' end
    end

    def var_of_format_or_suffix(str)
        if @@formats.key?(str) then
            @@formats[str]
        elsif @@suffixmap.key?(File.extname(str)) then # extname includes .
            @@formats[@@suffixmap[File.extname(str)]]  # extname includes .
        else
            ''
        end
    end

end

class KpseFast

    # test things

    def list_variables(kpseonly=true)
        @variables.keys.sort.each do |k|
            if kpseonly then
                puts("#{k} = #{@variables[k]}") if @kpsevars[k]
            else
                puts("#{if @kpsevars[k] then 'K' else 'E' end} #{k} = #{@variables[k]}")
            end
        end
    end

    def list_expansions(kpseonly=true)
        @expansions.keys.sort.each do |k|
            if kpseonly then
                puts("#{k} = #{@expansions[k]}") if @kpsevars[k]
            else
                puts("#{if @kpsevars[k] then 'K' else 'E' end} #{k} = #{@expansions[k]}")
            end
        end
    end

    def list_lsr
        puts("files = #{@files.size}")
    end

    def set_test_patterns
        @variables["KPSE_TEST_PATTERN_A"] = "foo/{1,2}/bar//"
        @variables["KPSE_TEST_PATTERN_B"] = "!!x{A,B{1,2}}y"
        @variables["KPSE_TEST_PATTERN_C"] = "x{A,B//{1,2}}y"
        @variables["KPSE_TEST_PATTERN_D"] = "x{A,B//{1,2,}}//y"
    end

    def show_test_patterns
        ['A','B','D'].each do |i|
            puts ""
            puts @variables ["KPSE_TEST_PATTERN_#{i}"]
            puts ""
            puts expand_path("KPSE_TEST_PATTERN_#{i}").split_path
            puts ""
        end
    end

end

class KpseFast

    # kpse stuff

    def expand_braces(str) # output variable and brace expansion of STRING.
        _expanded_path_(original_variable(str).split_path).join_path
    end

    def expand_path(str)   # output complete path expansion of STRING.
        _expanded_path_(expanded_variable(str).split_path).join_path
    end

    def expand_var(str)    # output variable expansion of STRING.
        expanded_variable(str)
    end

    def show_path(str)     # output search path for file type NAME
        expanded_path(str).join_path
    end

    def var_value(str)     # output the value of variable $STRING.
        original_variable(str)
    end

end

class KpseFast

    def _is_cnf_?(filename)
        filename == File.basename((@cnffiles.first rescue @@texmfcnf) || @@texmfcnf)
    end

    def find_file(filename)
        if _is_cnf_?(filename) then
            @cnffiles.first rescue ''
        else
            [find_files(filename,true)].flatten.first || ''
        end
    end

    def find_files(filename,first=false)
        if _is_cnf_?(filename) then
            result = @cnffiles.dup
        else
            if @remember then
                # stamp = "#{filename}--#{@format}--#{@engine}--#{@progname}"
                stamp = "#{filename}--#{@engine}--#{@progname}"
                return @found[stamp] if @found.key?(stamp)
            end
            pathlist = expanded_path(filename)
            result = []
            filelist = if @files.key?(filename) then @files[filename].uniq else nil end
            done = false
            if pathlist.size == 0 then
                if FileTest.file?(filename) then
                    done = true
                    result << '.'
                end
            else
                pathlist.each do |path|
                    doscan = if path =~ /^\!\!/o then false else true end
                    recurse = if path =~ /\/\/$/o then true else false end
                    pathname = path.dup
                    pathname.gsub!(/^\!+/o, '')
                    done = false
                    if not done and filelist then
                        # checking for exact match
                        if filelist.include?(pathname) then
                            result << pathname
                            done = true
                        end
                        if not done and recurse then
                            # checking for fuzzy //
                            pathname.gsub!(/\/+$/o, '/.*')
                            # pathname.gsub!(/\/\//o,'/[\/]*/')
                            pathname.gsub!(/\/\//o,'/.*?/')
                            re = /^#{pathname}/
                            filelist.each do |f|
                                if re =~ f then
                                    result << f # duplicates will be filtered later
                                    done = true
                                end
                                break if done
                            end
                        end
                    end
                    if not done and doscan then
                        # checking for path itself
                        pname = pathname.sub(/\.\*$/,'')
                        if not pname =~ /\*/o and FileTest.file?(File.join(pname,filename)) then
                            result << pname
                            done = true
                        end
                    end
                    break if done and first
                end
            end
            if not done and @scandisk then
                pathlist.each do |path|
                    pathname = path.dup
                    unless pathname.gsub!(/^\!+/o, '') then # !! prevents scan
                        recurse = pathname.gsub!(/\/+$/o, '')
                        complex = pathname.gsub!(/\/\//o,'/*/')
                        if recurse then
                            if complex then
                                if ok = File.glob_file("#{pathname}/**/#{filename}") then
                                    result << File.dirname(ok)
                                    done = true
                                end
                            elsif ok = File.locate_file(pathname,filename) then
                                result << File.dirname(ok)
                                done = true
                            end
                        elsif complex then
                            if ok = File.glob_file("#{pathname}/#{filename}") then
                                result << File.dirname(ok)
                                done = true
                            end
                        elsif FileTest.file?(File.join(pathname,filename)) then
                            result << pathname
                            done = true
                        end
                        break if done and first
                    end
                end
            end
            result = result.uniq.collect do |pathname|
                File.join(pathname,filename)
            end
            @found[stamp] = result if @remember
        end
        return result # redundant
    end

end

class KpseFast

    class FileData
        attr_accessor :tag, :name, :size, :date
        def initialize(tag=0,name=nil,size=nil,date=nil)
            @tag, @name, @size, @date = tag, name, size, date
        end
        def FileData.sizes(a)
            a.collect do |aa|
                aa.size
            end
        end
        def report
            case @tag
                when 1 then "deleted  | #{@size.to_s.rjust(8)} | #{@date.strftime('%m/%d/%Y %I:%M')} | #{@name}"
                when 2 then "present  | #{@size.to_s.rjust(8)} | #{@date.strftime('%m/%d/%Y %I:%M')} | #{@name}"
                when 3 then "obsolete | #{' '*8} | #{' '*16} | #{@name}"
            end
        end
    end

    def analyze_files(filter='',strict=false,sort='',delete=false)
        puts("command line     = #{ARGV.join(' ')}")
        puts("number of files  = #{@files.size}")
        puts("filter pattern   = #{filter}")
        puts("loaded cnf files = #{@cnffiles.join(' ')}")
        puts('')
        if filter.gsub!(/^not:/,'') then
            def the_same(filter,filename)
                not filter or filter.empty? or /#{filter}/ !~ filename
            end
        else
            def the_same(filter,filename)
                not filter or filter.empty? or /#{filter}/ =~ filename
            end
        end
        @files.keys.each do |name|
            if @files[name].size > 1 then
                data = Array.new
                @files[name].each do |path|
                    filename = File.join(path,name)
                    # if not filter or filter.empty? or /#{filter}/ =~ filename then
                    if the_same(filter,filename) then
                        if FileTest.file?(filename) then
                            if delete then
                                data << FileData.new(1,filename,File.size(filename),File.mtime(filename))
                                begin
                                    File.delete(filename) if delete
                                rescue
                                end
                            else
                                data << FileData.new(2,filename,File.size(filename),File.mtime(filename))
                            end
                        else
                            # data << FileData.new(3,filename)
                        end
                    end
                end
                if data.length > 1 then
                    if strict then
                        # if data.collect do |d| d.size end.uniq! then
                            # data.sort! do |a,b| b.size <=> a.size end
                            # data.each do |d| puts d.report end
                            # puts ''
                        # end
                        data.sort! do |a,b|
                            if a.size and b.size then
                                b.size <=> a.size
                            else
                                0
                            end
                        end
                        bunch = Array.new
                        done = false
                        data.each do |d|
                            if bunch.size == 0 then
                                bunch << d
                            elsif bunch[0].size == d.size then
                                bunch << d
                            else
                                if bunch.size > 1 then
                                    bunch.each do |b|
                                        puts b.report
                                    end
                                    done = true
                                end
                                bunch = [d]
                            end
                        end
                        puts '' if done
                    else
                        case sort
                            when 'size'    then data.sort! do |a,b| a.size <=> b.size end
                            when 'revsize' then data.sort! do |a,b| b.size <=> a.size end
                            when 'date'    then data.sort! do |a,b| a.date <=> b.date end
                            when 'revdate' then data.sort! do |a,b| b.date <=> a.date end
                        end
                        data.each do |d| puts d.report end
                        puts ''
                    end
                end
            end
        end
    end

end


    # k = KpseFast.new # (root)
    # k.set_test_patterns
    # k.load_cnf
    # k.expand_variables
    # k.load_lsr

    # k.show_test_patterns

    # puts k.list_variables
    # puts k.list_expansions
    # k.list_lsr
    # puts k.expansion("$TEXMF")
    # puts k.expanded_path("TEXINPUTS","context")

    # k.progname, k.engine, k.format = 'context', 'pdftex', 'tfm'
    # k.scandisk = false # == must_exist
    # k.expand_variables

    # 10.times do |i| puts k.find_file('texnansi-lmr10.tfm') end

    # puts "expand braces $TEXMF"
    # puts k.expand_braces("$TEXMF")
    # puts "expand path $TEXMF"
    # puts k.expand_path("$TEXMF")
    # puts "expand var $TEXMF"
    # puts k.expand_var("$TEXMF")
    # puts "expand path $TEXMF"
    # puts k.show_path('tfm')
    # puts "expand value $TEXINPUTS"
    # puts k.var_value("$TEXINPUTS")
    # puts "expand value $TEXINPUTS.context"
    # puts k.var_value("$TEXINPUTS.context")

    # exit



# kpse_merge_file: 'C:/data/develop/context/ruby/base/kpse/drb.rb'

require 'drb'
# kpse_merge_done: require 'base/kpse/trees'

class KpseServer

    attr_accessor :port

    def initialize(port=7000)
        @port = port
    end

    def start
        puts "starting drb service at port #{@port}"
        DRb.start_service("druby://localhost:#{@port}", KpseTrees.new)
        trap(:INT) do
            DRb.stop_service
        end
        DRb.thread.join
    end

    def stop
        # todo
    end

end

class KpseClient

    attr_accessor :port

    def initialize(port=7000)
        @port = port
        @kpse = nil
    end

    def start
        # only needed when callbacks are used / slow, due to Socket::getaddrinfo
        # DRb.start_service
    end

    def object
        @kpse = DRbObject.new(nil,"druby://localhost:#{@port}")
    end

end


# SERVER_URI="druby://localhost:8787"
#
#   # Start a local DRbServer to handle callbacks.
#   #
#   # Not necessary for this small example, but will be required
#   # as soon as we pass a non-marshallable object as an argument
#   # to a dRuby call.
#   DRb.start_service
#


# kpse_merge_file: 'C:/data/develop/context/ruby/base/kpse/trees.rb'

require 'monitor'
# kpse_merge_done: require 'base/kpsefast'

class KpseTrees < Monitor

    def initialize
        @trees = Hash.new
    end

    def pattern(filenames)
        filenames.join('|').gsub(/\\+/o,'/').downcase
    end

    def choose(filenames,environment)
        current = pattern(filenames)
        load(filenames,environment) unless @trees[current]
        puts "enabling tree #{current}"
        current
    end

    def fetch(filenames,environment) # will send whole object !
        current = pattern(filenames)
        load(filenames,environment) unless @trees[current]
        puts "fetching tree #{current}"
        @trees[current]
    end

    def load(filenames,environment)
        current = pattern(filenames)
        puts "loading tree #{current}"
        @trees[current] = KpseFast.new
        @trees[current].push_environment(environment)
        @trees[current].load_cnf(filenames)
        @trees[current].expand_variables
        @trees[current].load_lsr
    end

    def set(tree,key,value)
        case key
            when 'progname' then @trees[tree].progname = value
            when 'engine'   then @trees[tree].engine   = value
            when 'format'   then @trees[tree].format   = value
        end
    end
    def get(tree,key)
        case key
            when 'progname' then @trees[tree].progname
            when 'engine'   then @trees[tree].engine
            when 'format'   then @trees[tree].format
        end
    end

    def load_cnf(tree)
        @trees[tree].load_cnf
    end
    def load_lsr(tree)
        @trees[tree].load_lsr
    end
    def expand_variables(tree)
        @trees[tree].expand_variables
    end
    def expand_braces(tree,str)
        @trees[tree].expand_braces(str)
    end
    def expand_path(tree,str)
        @trees[tree].expand_path(str)
    end
    def expand_var(tree,str)
        @trees[tree].expand_var(str)
    end
    def show_path(tree,str)
        @trees[tree].show_path(str)
    end
    def var_value(tree,str)
        @trees[tree].var_value(str)
    end
    def find_file(tree,filename)
        @trees[tree].find_file(filename)
    end
    def find_files(tree,filename,first)
        @trees[tree].find_files(filename,first)
    end

end


# kpse_merge_file: 'C:/data/develop/context/ruby/base/kpsedirect.rb'

class KpseDirect

    attr_accessor :progname, :format, :engine

    def initialize
        @progname, @format, @engine = '', '', ''
    end

    def expand_path(str)
        clean_name(`kpsewhich -expand-path=#{str}`.chomp)
    end

    def expand_var(str)
        clean_name(`kpsewhich -expand-var=#{str}`.chomp)
    end

    def find_file(str)
        clean_name(`kpsewhich #{_progname_} #{_format_} #{str}`.chomp)
    end

    def _progname_
        if @progname.empty? then '' else "-progname=#{@progname}" end
    end
    def _format_
        if @format.empty?   then '' else "-format=\"#{@format}\"" end
    end

    private

    def clean_name(str)
        str.gsub(/\\/,'/')
    end

end


# kpse_merge_file: 'C:/data/develop/context/ruby/base/merge.rb'

# module    : base/merge
# copyright : PRAGMA Advanced Document Engineering
# version   : 2006
# author    : Hans Hagen
#
# project   : ConTeXt / eXaMpLe
# concept   : Hans Hagen
# info      : j.hagen@xs4all.nl


# this module will package all the used modules in the file itself
# so that we can relocate the file at wish, usage:
#
# merge:
#
# unless SelfMerge::ok? && SelfMerge::merge then
#     puts("merging should happen on the path were the base inserts reside")
# end
#
# cleanup:
#
# unless SelfMerge::cleanup then
#     puts("merging should happen on the path were the base inserts reside")

module SelfMerge

    @@kpsemergestart = "\# kpse_merge_start"
    @@kpsemergestop  = "\# kpse_merge_stop"
    @@kpsemergefile  = "\# kpse_merge_file: "
    @@kpsemergedone  = "\# kpse_merge_done: "

    @@filename = File.basename($0)
    @@ownpath  = File.expand_path(File.dirname($0))
    @@modroot  = '(base|graphics|rslb|www)' # needed in regex in order not to mess up SelfMerge
    @@modules  = $".collect do |file| File.expand_path(file) end

    @@modules.delete_if do |file|
        file !~ /^#{@@ownpath}\/#{@@modroot}.*$/
    end

    def SelfMerge::ok?
        begin
            @@modules.each do |file|
                return false unless FileTest.file?(file)
            end
        rescue
            return false
        else
            return true
        end
    end

    def SelfMerge::merge
        begin
            if SelfMerge::ok? && rbfile = IO.read(@@filename) then
                begin
                    inserts = "#{@@kpsemergestart}\n\n"
                    @@modules.each do |file|
                        inserts << "#{@@kpsemergefile}'#{file}'\n\n"
                        inserts << IO.read(file).gsub(/^#.*?\n$/,'')
                        inserts << "\n\n"
                    end
                    inserts << "#{@@kpsemergestop}\n\n"
                    # no gsub! else we end up in SelfMerge
                    rbfile.sub!(/#{@@kpsemergestart}\s*#{@@kpsemergestop}/mois) do
                        inserts
                    end
                    rbfile.gsub!(/^(.*)(require [\"\'].*?#{@@modroot}.*)$/) do
                        pre, post = $1, $2
                        if pre =~ /#{@@kpsemergedone}/ then
                            "#{pre}#{post}"
                        else
                            "#{pre}#{@@kpsemergedone}#{post}"
                        end
                    end
                rescue
                    return false
                else
                    begin
                        File.open(@@filename,'w') do |f|
                            f << rbfile
                        end
                    rescue
                        return false
                    end
                end
            end
        rescue
            return false
        else
            return true
        end
    end

    def SelfMerge::cleanup
        begin
            if rbfile = IO.read(@@filename) then
                begin
                    rbfile.sub!(/#{@@kpsemergestart}(.*)#{@@kpsemergestop}\s*/mois) do
                        "#{@@kpsemergestart}\n\n#{@@kpsemergestop}\n\n"
                    end
                    rbfile.gsub!(/^(.*#{@@kpsemergedone}.*)$/) do
                        str = $1
                        if str =~ /require [\"\']/ then
                            str.gsub(/#{@@kpsemergedone}/, '')
                        else
                            str
                        end
                    end
                rescue
                    return false
                else
                    begin
                        File.open(@@filename,'w') do |f|
                            f << rbfile
                        end
                    rescue
                        return false
                    end
                end
            end
        rescue
            return false
        else
            return true
        end
    end

    def SelfMerge::replace
        if SelfMerge::ok? then
            SelfMerge::cleanup
            SelfMerge::merge
        end
    end

end


# kpse_merge_stop



$mswindows = Config::CONFIG['host_os'] =~ /mswin/
$separator = File::PATH_SEPARATOR
$version   = "2.0.3"
$ownpath   = File.dirname($0)

if $mswindows then
    require "win32ole"
    require "Win32API"
end

# exit if defined?(REQUIRE2LIB)

$stdout.sync = true
$stderr.sync = true

$applications = Hash.new
$suffixinputs = Hash.new
$predefined   = Hash.new

$suffixinputs['pl']  = 'PERLINPUTS'
$suffixinputs['rb']  = 'RUBYINPUTS'
$suffixinputs['py']  = 'PYTHONINPUTS'
$suffixinputs['lua'] = 'LUAINPUTS'
$suffixinputs['jar'] = 'JAVAINPUTS'
$suffixinputs['pdf'] = 'PDFINPUTS'

$predefined['texexec']  = 'texexec.rb'
$predefined['texutil']  = 'texutil.rb'
$predefined['texfont']  = 'texfont.pl'
$predefined['texshow']  = 'texshow.pl'

$predefined['makempy']  = 'makempy.pl'
$predefined['mptopdf']  = 'mptopdf.pl'
$predefined['pstopdf']  = 'pstopdf.rb'

$predefined['examplex'] = 'examplex.rb'
$predefined['concheck'] = 'concheck.rb'

$predefined['runtools'] = 'runtools.rb'
$predefined['textools'] = 'textools.rb'
$predefined['tmftools'] = 'tmftools.rb'
$predefined['ctxtools'] = 'ctxtools.rb'
$predefined['rlxtools'] = 'rlxtools.rb'
$predefined['pdftools'] = 'pdftools.rb'
$predefined['mpstools'] = 'mpstools.rb'
$predefined['exatools'] = 'exatools.rb'
$predefined['xmltools'] = 'xmltools.rb'
$predefined['luatools'] = 'luatools.lua'
$predefined['mtxtools'] = 'mtxtools.rb'

$predefined['newpstopdf']   = 'pstopdf.rb'
$predefined['newtexexec']   = 'texexec.rb'
$predefined['pdftrimwhite'] = 'pdftrimwhite.pl'

$makelist = [
    # context
    'texexec',
    'texutil',
    'texfont',
    # mp/ps
    'pstopdf',
    'mptopdf',
    'makempy',
    # misc
    'ctxtools',
    'pdftools',
    'xmltools',
    'textools',
    'mpstools',
    'tmftools',
    'exatools',
    'runtools',
    'rlxtools',
    'luatools',
    'mtxtools',
    #
    # no, 'texmfstart'
]

$scriptlist   = 'rb|pl|py|lua|jar'
$documentlist = 'pdf|ps|eps|htm|html'

$editor       = ENV['TEXMFSTART_EDITOR'] || ENV['EDITOR'] || ENV['editor'] || 'scite'

$crossover    = true # to other tex tools, else only local
$kpse         = nil

def set_applications(page=1)

    $applications['unknown']  = ''
    $applications['perl']     = $applications['pl']  = 'perl'
    $applications['ruby']     = $applications['rb']  = 'ruby'
    $applications['python']   = $applications['py']  = 'python'
    $applications['lua']      = $applications['lua'] = 'luatex --luaonly'
    $applications['java']     = $applications['jar'] = 'java'

    if $mswindows then
        $applications['pdf']  = ['',"pdfopen --page #{page} --file",'acroread']
        $applications['html'] = ['','netscape','mozilla','opera','iexplore']
        $applications['ps']   = ['','gview32','gv','gswin32','gs']
    else
        $applications['pdf']  = ["pdfopen --page #{page} --file",'acroread']
        $applications['html'] = ['netscape','mozilla','opera']
        $applications['ps']   = ['gview','gv','gs']
    end

    $applications['htm']      = $applications['html']
    $applications['eps']      = $applications['ps']

end

set_applications()

def check_kpse
    if $kpse then
        # already done
    else
        begin
            if KpseRemote::available? then
                $kpse = KpseRemote.new
                if $kpse.okay? then
                    puts("kpse     : remote") if $verbose
                else
                    $kpse = KpseDirect.new
                    puts("kpse     : direct (forced)") if $verbose
                end
            else
                $kpse = KpseDirect.new
                puts("kpse     : direct") if $verbose
            end
        rescue
            puts("kpse     : direct (fallback)") if $verbose
        end
    end
end

if $mswindows then

    GetShortPathName = Win32API.new('kernel32', 'GetShortPathName', ['P','P','N'], 'N')
    GetLongPathName  = Win32API.new('kernel32', 'GetLongPathName',  ['P','P','N'], 'N')

    def dowith_pathname (filename,filemethod)
        filename = filename.gsub(/\\/o,'/') # no gsub! because filename can be frozen
        case filename
            when /\;/o then
                # could be a path spec
                return filename
            when /\s+/o then
                # danger lurking
                buffer = ' ' * 260
                length = filemethod.call(filename,buffer,buffer.size)
                if length>0 then
                    return buffer.slice(0..length-1)
                else
                    # when the path or file does not exist, nothing is returned
                    # so we try to handle the path separately from the basename
                    basename = File.basename(filename)
                    pathname = File.dirname(filename)
                    length = filemethod.call(pathname,buffer,260)
                    if length>0 then
                        return buffer.slice(0..length-1) + '/' + basename
                    else
                        return filename
                    end
                end
            else
                # no danger
                return filename
        end
    end

    def longpathname (filename)
        dowith_pathname(filename,GetLongPathName)
    end

    def shortpathname (filename)
        dowith_pathname(filename,GetShortPathName)
    end

else

    def longpathname (filename)
        filename
    end

    def shortpathname (filename)
        filename
    end

end

class File

    # def File.needsupdate(oldname,newname)
        # begin
            # if $mswindows then
                # return File.stat(oldname).mtime > File.stat(newname).mtime
            # else
                # return File.stat(oldname).mtime != File.stat(newname).mtime
            # end
        # rescue
            # return true
        # end
    # end

    @@update_eps = 1

    def File.needsupdate(oldname,newname)
        begin
            oldtime = File.stat(oldname).mtime.to_i
            newtime = File.stat(newname).mtime.to_i
            if newtime >= oldtime then
                return false
            elsif oldtime-newtime < @@update_eps then
                return false
            else
                return true
            end
        rescue
            return true
        end
    end

    def File.syncmtimes(oldname,newname)
        return
        begin
            if $mswindows then
                # does not work (yet) / gives future timestamp
                # t = File.mtime(oldname) # i'm not sure if the time is frozen, so we do it here
                # File.utime(0,t,oldname,newname)
            else
                t = File.mtime(oldname) # i'm not sure if the time is frozen, so we do it here
                File.utime(0,t,oldname,newname)
            end
        rescue
        end
    end

    def File.timestamp(name)
        begin
            "#{File.stat(name).mtime}"
        rescue
            return 'unknown'
        end
    end

end

def hashed (arr=[])
    arg = if arr.class == String then arr.split(' ') else arr.dup end
    hsh = Hash.new
    if arg.length > 0
        hsh['arguments'] = ''
        done = false
        arg.each do |s|
            if done then
                hsh['arguments'] += ' ' + s
            else
                kvl = s.split('=')
                if kvl[0].sub!(/^\-+/,'') then
                    hsh[kvl[0]] = if kvl.length > 1 then kvl[1] else true end
                else
                    hsh['file'] = s
                    done = true
                end
            end
        end
    end
    return hsh
end

def launch(filename)
    if $browser && $mswindows then
        filename = filename.gsub(/\.[\/\\]/) do
            Dir.getwd + '/'
        end
        report("launching #{filename}")
        ie = WIN32OLE.new("InternetExplorer.Application")
        ie.visible = true
        ie.navigate(filename)
        return true
    else
        return false
    end
end

# env|environment
# rel|relative
# loc|locate|kpse|path|file

def expanded(arg) # no "other text files", too restricted
    arg.gsub(/(env|environment)\:([a-zA-Z\-\_\.0-9]+)/o) do
        method, original, resolved = $1, $2, ''
        if resolved = ENV[original] then
            report("environment variable #{original} expands to #{resolved}") unless $report
            resolved
        else
            report("environment variable #{original} cannot be resolved") unless $report
            original
        end
    end . gsub(/(rel|relative)\:([a-zA-Z\-\_\.0-9]+)/o) do
        method, original, resolved = $1, $2, ''
        ['.','..','../..'].each do |r|
            if FileTest.file?(File.join(r,original)) then
                resolved = File.join(r,original)
                break
            end
        end
        if resolved.empty? then
            original
        else
            resolved
        end
    end . gsub(/(kpse|loc|locate|file|path)\:([a-zA-Z\-\_\.0-9]+)/o) do
        method, original, resolved = $1, $2, ''
        if $program && ! $program.empty? then
            # pstrings = ["-progname=#{$program}"]
            pstrings = [$program]
        else
            # pstrings = ['','-progname=context']
            pstrings = ['','context']
        end
        # auto suffix with texinputs as fall back
        if ENV["_CTX_K_V_#{original}_"] then
            resolved = ENV["_CTX_K_V_#{original}_"]
            report("environment provides #{original} as #{resolved}") unless $report
            resolved
        else
            check_kpse
            pstrings.each do |pstr|
                if resolved.empty? then
                    # command = "kpsewhich #{pstr} #{original}"
                    # report("running #{command}")
                    report("locating '#{original}' in program space '#{pstr}'")
                    begin
                        # resolved = `#{command}`.chomp
                        $kpse.progname = pstr
                        $kpse.format = ''
                        resolved = $kpse.find_file(original).gsub(/\\/,'/')
                    rescue
                        resolved = ''
                    end
                end
                # elsewhere in the tree
                if resolved.empty? then
                    # command = "kpsewhich #{pstr} -format=\"other text files\" #{original}"
                    # report("running #{command}")
                    report("locating '#{original}' in program space '#{pstr}' using format 'other text files'")
                    begin
                        # resolved = `#{command}`.chomp
                        $kpse.progname = pstr
                        $kpse.format = 'other text files'
                        resolved = $kpse.find_file(original).gsub(/\\/,'/')
                    rescue
                        resolved = ''
                    end
                end
            end
            if resolved.empty? then
                original = File.dirname(original) if method =~ /path/
                report("#{original} is not resolved") unless $report
                ENV["_CTX_K_V_#{original}_"] = original if $crossover
                original
            else
                resolved = File.dirname(resolved) if method =~ /path/
                report("#{original} is resolved to #{resolved}") unless $report
                ENV["_CTX_K_V_#{original}_"] = resolved if $crossover
                resolved
            end
        end
    end
end

def changeddir?(path)
    if path.empty? then
        return true
    else
        oldpath = File.expand_path(path)
        begin
            Dir.chdir(path) if not path.empty?
        rescue
            report("unable to change to directory: #{path}")
        else
            report("changed to directory: #{path}")
        end
        newpath = File.expand_path(Dir.getwd)
        return oldpath == newpath
    end
end

def runcommand(command)
    if $locate then
        command = command.split(' ').collect do |c|
            if c =~ /\//o then
                begin
                    cc = File.expand_path(c)
                    c = cc if FileTest.file?(cc)
                rescue
                end
            end
            c
        end . join(' ')
        print command # to stdout and no newline
    elsif $execute then
        report("using 'exec' instead of 'system' call: #{command}")
        exec(command) if changeddir?($path)
    else
        report("using 'system' call: #{command}")
        system(command) if changeddir?($path)
    end
end

def runoneof(application,fullname,browserpermitted)
    if browserpermitted && launch(fullname) then
        return true
    else
        report("starting #{$filename}") unless $report
        output("\n") if $report && $verbose
        applications = $applications[application.downcase]
        if ! applications then
            output("problems with determining application type")
            return true
        elsif applications.class == Array then
            if $report then
                output([fullname,expanded($arguments)].join(' '))
                return true
            else
                applications.each do |a|
                    return true if runcommand([a,fullname,expanded($arguments)].join(' '))
                end
            end
        elsif applications.empty? then
            if $report then
                output([fullname,expanded($arguments)].join(' '))
                return true
            else
                return runcommand([fullname,expanded($arguments)].join(' '))
            end
        else
            if $report then
                output([applications,fullname,expanded($arguments)].join(' '))
                return true
            else
                return runcommand([applications,fullname,expanded($arguments)].join(' '))
            end
        end
        return false
    end
end

def report(str)
    $stdout.puts(str) if $verbose
end

def output(str)
    $stdout.puts(str)
end

def usage
    print "version  : #{$version} - 2003/2006 - www.pragma-ade.com\n"
    print("\n")
    print("usage    : texmfstart [switches] filename [optional arguments]\n")
    print("\n")
    print("switches : --verbose --report --browser --direct --execute --locate --iftouched --ifchanged\n")
    print("           --program --file --page --arguments --batch --edit --report --clear\n")
    print("           --make --lmake --wmake --path --stubpath --indirect --before --after\n")
    print("           --tree --autotree --environment --showenv\n")
    print("\n")
    print("example  : texmfstart pstopdf.rb cow.eps\n")
    print("           texmfstart --locate examplex.rb\n")
    print("           texmfstart --execute examplex.rb\n")
    print("           texmfstart --browser examplap.pdf\n")
    print("           texmfstart showcase.pdf\n")
    print("           texmfstart --page=2 --file=showcase.pdf\n")
    print("           texmfstart --program=yourtex yourscript.rb arg-1 arg-2\n")
    print("           texmfstart --direct xsltproc kpse:somefile.xsl somefile.xml\n")
    print("           texmfstart --direct ruby rel:wn-cleanup-1.rb oldfile.xml newfile.xml\n")
    print("           texmfstart bin:xsltproc env:somepreset path:somefile.xsl somefile.xml\n")
    print("           texmfstart --iftouched=normal,lowres downsample.rb normal lowres\n")
    print("           texmfstart --ifchanged=somefile.dat --direct processit somefile.dat\n")
    print("           texmfstart bin:scite kpse:texmf.cnf\n")
    print("           texmfstart --exec bin:scite *.tex\n")
    print("           texmfstart --edit texmf.cnf\n")
    print("           texmfstart --edit kpse:texmf.cnf\n")
    print("           texmfstart --serve\n")
    print("\n")
    print("           texmfstart --stubpath=/usr/local/bin [--make --remove] --verbose all\n")
    print("           texmfstart --stubpath=auto [--make --remove] all\n")
    print("\n")
    check_kpse
end

# somehow registration does not work out (at least not under windows)

def tag(name)
    if $crossover then "_CTX_K_S_#{name}_" else "TEXMFSTART.#{name}" end
end

def registered?(filename)
    return ENV[tag(filename)] != nil
end

def registered(filename)
    return ENV[tag(filename)] || 'unknown'
end

def register(filename,fullname)
    if fullname && ! fullname.empty? then # && FileTest.file?(fullname)
        ENV[tag(filename)] = fullname
        report("registering '#{filename}' as '#{fullname}'")
        return true
    else
        return false
    end
end

def find(filename,program)
    begin
        filename = filename.sub(/script:/o, '') # so we have bin: and script: and nothing
        if $predefined.key?(filename) then
            report("expanding '#{filename}' to '#{$predefined[filename]}'")
            filename = $predefined[filename]
        end
        if registered?(filename) then
            report("already located '#{filename}'")
            return registered(filename)
        end
        # create suffix list
        if filename =~ /^(.*)\.(.+)$/ then
            filename = $1
            suffixlist = [$2]
        else
            suffixlist = [$scriptlist.split('|'),$documentlist.split('|')].flatten
        end
        # first we honor a given path
        if filename =~ /[\\\/]/ then
            report("trying to honor '#{filename}'")
            suffixlist.each do |suffix|
                fullname = filename+'.'+suffix
                if FileTest.file?(fullname) && register(filename,fullname)
                    return shortpathname(fullname)
                end
            end
        end
        filename.sub!(/^.*[\\\/]/, '')
        # next we look at the current path and the callerpath
        pathlist = [ ]
        progpath = $applications[suffixlist[0]]
        threadok = registered("THREAD") !~ /unknown/
        pathlist << ['.','current']
        pathlist << [$ownpath,'caller']                                 if $ownpath != '.'
        pathlist << ["#{$ownpath}/../#{progpath}",'caller']             if progpath
        pathlist << [registered("THREAD"),'thread']                     if threadok
        pathlist << ["#{registered("THREAD")}/../#{progpath}",'thread'] if progpath && threadok
        pathlist.each do |p|
            if p && ! p.empty? && ! (p[0] == 'unknown') then
                suffixlist.each do |suffix|
                    fname = "#{filename}.#{suffix}"
                    fullname = File.expand_path(File.join(p[0],fname))
                    report("locating '#{fname}' in #{p[1]} path '#{p[0]}'")
                    if FileTest.file?(fullname) && register(filename,fullname) then
                        report("'#{fname}' located in #{p[1]} path")
                        return shortpathname(fullname)
                    end
                end
            end
        end
        # now we consult environment settings
        fullname = nil
        check_kpse
        $kpse.progname = program
        suffixlist.each do |suffix|
            begin
                break unless $suffixinputs[suffix]
                environment = ENV[$suffixinputs[suffix]] || ENV[$suffixinputs[suffix]+".#{$program}"]
                if ! environment || environment.empty? then
                    begin
                        # environment = `kpsewhich -expand-path=\$#{$suffixinputs[suffix]}`.chomp
                        environment = $kpse.expand_path("\$#{$suffixinputs[suffix]}")
                    rescue
                        environment = nil
                    else
                        if environment && ! environment.empty? then
                            report("using kpsewhich variable #{$suffixinputs[suffix]}")
                        end
                    end
                elsif environment && ! environment.empty? then
                    report("using environment variable #{$suffixinputs[suffix]}")
                end
                if environment && ! environment.empty? then
                    environment.split($separator).each do |e|
                        e.strip!
                        e = '.' if e == '\.' # somehow . gets escaped
                        e += '/' unless e =~ /[\\\/]$/
                        fullname = e + filename + '.' + suffix
                        report("testing '#{fullname}'")
                        if FileTest.file?(fullname) then
                            break
                        else
                            fullname = nil
                        end
                    end
                end
            rescue
                report("environment string '#{$suffixinputs[suffix]}' cannot be used to locate '#{filename}'")
                fullname = nil
            else
                return shortpathname(fullname) if register(filename,fullname)
            end
        end
        return shortpathname(fullname) if register(filename,fullname)
        # then we fall back on kpsewhich
        suffixlist.each do |suffix|
            # TDS script scripts location as per 2004
            if suffix =~ /(#{$scriptlist})/ then
                begin
                    report("using 'kpsewhich' to locate '#{filename}' in suffix space '#{suffix}' (1)")
                    # fullname = `kpsewhich -progname=#{program} -format=texmfscripts #{filename}.#{suffix}`.chomp
                    $kpse.format = 'texmfscripts'
                    fullname = $kpse.find_file("#{filename}.#{suffix}").gsub(/\\/,'/')
                rescue
                    report("kpsewhich cannot locate '#{filename}' in suffix space '#{suffix}' (1)")
                    fullname = nil
                else
                    return shortpathname(fullname) if register(filename,fullname)
                end
            end
            # old TDS location: .../texmf/context/...
            begin
                report("using 'kpsewhich' to locate '#{filename}' in suffix space '#{suffix}' (2)")
                # fullname = `kpsewhich -progname=#{program} -format="other text files" #{filename}.#{suffix}`.chomp
                $kpse.format = 'other text files'
                fullname = $kpse.find_file("#{filename}.#{suffix}").gsub(/\\/,'/')
            rescue
                report("kpsewhich cannot locate '#{filename}' in suffix space '#{suffix}' (2)")
                fullname = nil
            else
                return shortpathname(fullname) if register(filename,fullname)
            end
        end
        return shortpathname(fullname) if register(filename,fullname)
        # let's take a look at the path
        paths = ENV['PATH'].split($separator)
        suffixlist.each do |s|
            paths.each do |p|
                suffixedname = "#{filename}.#{s}"
                report("checking #{p} for #{filename}")
                if FileTest.file?(File.join(p,suffixedname)) then
                    fullname = File.join(p,suffixedname)
                    return  shortpathname(fullname) if register(filename,fullname)
                end
            end
        end
        # bad luck, we need to search the tree ourselves
        if (suffixlist.length == 1) && (suffixlist.first =~ /(#{$documentlist})/) then
            report("aggressively locating '#{filename}' in document trees")
            begin
                # texroot = `kpsewhich -expand-var=$SELFAUTOPARENT`.chomp
                texroot = $kpse.expand_var("$SELFAUTOPARENT")
            rescue
                texroot = ''
            else
                texroot.sub!(/[\\\/][^\\\/]*?$/, '')
            end
            if not texroot.empty? then
                sffxlst = suffixlist.join(',')
                begin
                    report("locating '#{filename}' in document tree '#{texroot}/doc*'")
                    if (result = Dir.glob("#{texroot}/doc*/**/#{filename}.{#{sffxlst}}")) && result && result[0] && FileTest.file?(result[0]) then
                        fullname = result[0]
                    end
                rescue
                    report("locating '#{filename}.#{suffixlist.join('|')}' in tree '#{texroot}' aborted")
                end
            end
            return shortpathname(fullname) if register(filename,fullname)
        end
        report("aggressively locating '#{filename}' in tex trees")
        begin
            # textrees = `kpsewhich -expand-var=$TEXMF`.chomp
            textrees = $kpse.expand_var("$TEXMF")
        rescue
            textrees = ''
        end
        if not textrees.empty? then
            textrees.gsub!(/[\{\}\!]/, '')
            textrees = textrees.split(',')
            if (suffixlist.length == 1) && (suffixlist.first =~ /(#{$documentlist})/) then
                speedup = ['doc**','**']
            else
                speedup = ['**']
            end
            sffxlst = suffixlist.join(',')
            speedup.each do |speed|
                textrees.each do |tt|
                    tt.gsub!(/[\\\/]$/, '')
                    if FileTest.directory?(tt) then
                        begin
                            report("locating '#{filename}' in tree '#{tt}/#{speed}/#{filename}.{#{sffxlst}}'")
                            if (result = Dir.glob("#{tt}/#{speed}/#{filename}.{#{sffxlst}}")) && result && result[0] && FileTest.file?(result[0]) then
                                fullname = result[0]
                                break
                            end
                        rescue
                            report("locating '#{filename}' in tree '#{tt}' aborted")
                            next
                        end
                    end
                end
                break if fullname && ! fullname.empty?
            end
        end
        if register(filename,fullname) then
            return shortpathname(fullname)
        else
            return ''
        end
    rescue
        error, trace = $!, $@.join("\n")
        report("fatal error: #{error}\n#{trace}")
        # report("fatal error")
    end
end

def run(fullname)
    if ! fullname || fullname.empty? then
        output("the file '#{$filename}' is not found")
    elsif FileTest.file?(fullname) then
        begin
            case fullname
                when /\.(#{$scriptlist})$/i then
                    return runoneof($1,fullname,false)
                when /\.(#{$documentlist})$/i then
                    return runoneof($1,fullname,true)
                else
                    return runoneof('unknown',fullname,false)
            end
        rescue
            report("starting '#{$filename}' in program space '#{$program}' fails (#{$!})")
        end
    else
        report("the file '#{$filename}' in program space '#{$program}' is not accessible")
    end
    return false
end

def direct(fullname)
    begin
        return runcommand([fullname.sub(/^(bin|binary)\:/, ''),expanded($arguments)].join(' '))
    rescue
        return false
    end
end

def edit(filename)
    begin
        return runcommand([$editor,expanded(filename),expanded($arguments)].join(' '))
    rescue
        return false
    end
end

def make(filename,windows=false,linux=false,remove=false)
    basename = File.basename(filename).gsub(/\.[^.]+?$/, '')
    if $stubpath == 'auto' then
        basename = File.dirname($0) + '/' + basename
    else
        basename = $stubpath + '/' + basename unless $stubpath.empty?
    end
    if filename == 'texmfstart' then
        program = 'ruby'
        command = 'kpsewhich --format=texmfscripts --progname=context texmfstart.rb'
        filename = `#{command}`.chomp.gsub(/\\/, '/')
        if filename.empty? then
            report("failure: #{command}")
            return
        elsif not remove then
            if windows then
                ['bat','exe'].each do |suffix|
                    if FileTest.file?("#{basename}.#{suffix}") then
                        report("windows stub '#{basename}.#{suffix}' skipped (already present)")
                        return
                    end
                end
            elsif linux && FileTest.file?(basename) then
                report("unix stub '#{basename}' skipped (already present)")
                return
            end
        end
    else
        program = nil
        if filename =~ /[\\\/]/ && filename =~ /\.(#{$scriptlist})$/ then
            program = $applications[$1]
        end
        filename = "\"#{filename}\"" if filename =~ /\s/
        program = 'texmfstart' if $indirect || ! program || program.empty?
    end
    begin
        callname = $predefined[filename.sub(/\.*?$/,'')] || filename
        if remove then
            if windows && (File.delete(basename+'.bat') rescue false) then
                report("windows stub '#{basename}.bat' removed (calls #{callname})")
            elsif linux && (File.delete(basename) rescue false) then
                report("unix stub '#{basename}' removed (calls #{callname})")
            end
        else
            if windows && f = open(basename+'.bat','w') then
                f.binmode
                f.write("@echo off\015\012")
                f.write("#{program} #{callname} %*\015\012")
                f.close
                report("windows stub '#{basename}.bat' made (calls #{callname})")
            elsif linux && f = open(basename,'w') then
                f.binmode
                f.write("#!/bin/sh\012")
                f.write("#{program} #{callname} \"$@\"\012")
                f.close
                report("unix stub '#{basename}' made (calls #{callname})")
            end
        end
    rescue
        report("failed to make stub '#{basename}' #{$!}")
        return false
    else
        return true
    end
end

def process(&block)
    if $iftouched then
        files = $directives['iftouched'].split(',')
        oldname, newname = files[0], files[1]
        if oldname && newname && File.needsupdate(oldname,newname) then
            report("file #{oldname}: #{File.timestamp(oldname)}")
            report("file #{newname}: #{File.timestamp(newname)}")
            report("file is touched, processing started")
            yield
            File.syncmtimes(oldname,newname)
        else
            report("file #{oldname} is untouched")
        end
    elsif $ifchanged then
        filename = $directives['ifchanged']
        checkname = filename + ".md5"
        oldchecksum, newchecksum = "old", "new"
        begin
            newchecksum = MD5.new(IO.read(filename)).hexdigest.upcase
        rescue
            newchecksum = "new"
        else
            begin
                oldchecksum = IO.read(checkname).chomp
            rescue
                oldchecksum = "old"
            end
        end
        if $verbose then
            report("old checksum #{filename}: #{oldchecksum}")
            report("new checksum #{filename}: #{newchecksum}")
        end
        if oldchecksum != newchecksum then
            report("file is changed, processing started")
            begin
                File.open(checkname,'w') do |f|
                    f << newchecksum
                end
            rescue
            end
            yield
        else
            report("file #{filename} is unchanged")
        end
    else
        yield
    end
end

def checkenvironment(tree)
    report('')
    ENV['TMP'] = ENV['TMP'] || ENV['TEMP'] || ENV['TMPDIR'] || ENV['HOME']
    case RUBY_PLATFORM
        when /(mswin|bccwin|mingw|cygwin)/i then ENV['TEXOS'] = ENV['TEXOS'] || 'texmf-mswin'
        when /(linux)/i                     then ENV['TEXOS'] = ENV['TEXOS'] || 'texmf-linux'
        when /(darwin|rhapsody|nextstep)/i  then ENV['TEXOS'] = ENV['TEXOS'] || 'texmf-macosx'
    #   when /(netbsd|unix)/i               then # todo
        else                                     # todo
    end
    ENV['TEXOS']   = "#{ENV['TEXOS'].sub(/^[\\\/]*/, '').sub(/[\\\/]*$/, '')}"
    ENV['TEXPATH'] = tree.sub(/\/+$/,'') # + '/'
    ENV['TEXMFOS'] = "#{ENV['TEXPATH']}/#{ENV['TEXOS']}"
    report('')
    report("preset : TEXPATH => #{ENV['TEXPATH']}")
    report("preset : TEXOS   => #{ENV['TEXOS']}")
    report("preset : TEXMFOS => #{ENV['TEXMFOS']}")
    report("preset : TMP => #{ENV['TMP']}")
    report('')
end

def loadfile(filename)
    begin
        IO.readlines(filename).each do |line|
            case line.chomp
                when /^[\#\%]/ then
                    # comment
                when /^(.*?)\s*(\>|\=|\<)\s*(.*)\s*$/ then
                    # = assign | > prepend | < append
                    key, how, value = $1, $2, $3
                    begin
                        # $SAFE = 0
                        value.gsub!(/\%(.*?)\%/) do
                            ENV[$1] || ''
                        end
                        # value.gsub!(/\;/,$separator) if key =~ /PATH/i then
                        case how
                            when '=', '<<' then ENV[key] = value
                            when '?', '??' then ENV[key] = ENV[key] || value
                            when '<', '+=' then ENV[key] = (ENV[key] || '') + $separator + value
                            when '>', '=+' then ENV[key] = value + $separator + (ENV[key] ||'')
                        end
                    rescue
                        report("user set failed : #{key} (#{$!})")
                    else
                        report("user set : #{key} => #{ENV[key]}")
                    end
            end
        end
    rescue
        report("error in reading file '#{filename}'")
    end
end

def loadtree(tree)
    begin
        unless tree.empty? then
            if File.directory?(tree) then
                setuptex = File.join(tree,'setuptex.tmf')
            else
                setuptex = tree.dup
            end
            if FileTest.file?(setuptex) then
                report("tex tree definition: #{setuptex}")
                checkenvironment(File.dirname(setuptex))
                loadfile(setuptex)
            else
                report("no setup file '#{setuptex}'")
            end
        end
    rescue
        # maybe tree is empty or boolean (no arg given)
    end
end

def loadenvironment(environment)
    begin
        unless environment.empty? then
            filename = if $path.empty? then environment else File.expand_path(File.join($path,environment)) end
            if FileTest.file?(filename) then
                report("environment : #{environment}")
                loadfile(filename)
            else
                report("no environment file '#{environment}'")
            end
        end
    rescue
        report("problem while loading '#{environment}'")
    end
end

def show_environment
    if $showenv then
        keys = ENV.keys.sort
        size = 0
        keys.each do |k|
            size = k.size if k.size > size
        end
        report('')
        keys.each do |k|
            report("#{k.rjust(size)} => #{ENV[k]}")
        end
        report('')
    end
end

def execute(arguments)

    arguments = arguments.split(/\s+/) if arguments.class == String

    $directives = hashed(arguments)

    $help        = $directives['help']        || false
    $batch       = $directives['batch']       || false
    $filename    = $directives['file']        || ''
    $program     = $directives['program']     || 'context'
    $direct      = $directives['direct']      || false
    $edit        = $directives['edit']        || false
    $page        = $directives['page']        || 1
    $browser     = $directives['browser']     || false
    $report      = $directives['report']      || false
    $verbose     = $directives['verbose']     || false
    $arguments   = $directives['arguments']   || ''
    $execute     = $directives['execute']     || $directives['exec'] || false
    $locate      = $directives['locate']      || false

    $autotree    = if $directives['autotree'] then (ENV['TEXMFSTART_TREE'] || ENV['TEXMFSTARTTREE'] || '') else '' end

    $path        = $directives['path']        || ''
    $tree        = $directives['tree']        || $autotree || ''
    $environment = $directives['environment'] || ''

    $make        = $directives['make']        || false
    $remove      = $directives['remove']      || $directives['delete'] || false
    $unix        = $directives['unix']        || false
    $windows     = $directives['windows']     || $directives['mswin'] || false
    $stubpath    = $directives['stubpath']    || ''
    $indirect    = $directives['indirect']    || false

    $before      = $directives['before']      || ''
    $after       = $directives['after']       || ''

    $iftouched   = $directives['iftouched']   || false
    $ifchanged   = $directives['ifchanged']   || false

    $openoffice  = $directives['oo']          || false

    $crossover   = false if $directives['clear']

    $showenv     = $directives['showenv']     || false
    $verbose     = true if $showenv

    $serve       = $directives['serve']       || false

    $verbose = true if (ENV['_CTX_VERBOSE_'] =~ /(y|yes|t|true|on)/io) && ! $locate && ! $report

    set_applications($page)

    # private:

    $selfmerge   = $directives['selfmerge'] || false
    $selfcleanup = $directives['selfclean'] || $directives['selfcleanup'] || false

    ENV['_CTX_VERBOSE_'] = 'yes' if $verbose

    if $openoffice then
        if ENV['OOPATH'] then
            if FileTest.directory?(ENV['OOPATH']) then
                report("using open office python")
                if $mswindows then
                    $applications['python'] = $applications['py']  = "\"#{File.join(ENV['OOPATH'],'program','python.bat')}\""
                else
                    $applications['python'] = $applications['py']  = File.join(ENV['OOPATH'],'python')
                end
                report("python path #{$applications['python']}")
            else
                report("environment variable 'OOPATH' does not exist")
            end
        else
            report("environment variable 'OOPATH' is not set")
        end
    end

    if $selfmerge then
        output("ruby libraries are cleaned up") if SelfMerge::cleanup
        output("ruby libraries are merged")     if SelfMerge::merge
        return true
    elsif $selfcleanup then
        output("ruby libraries are cleaned up") if SelfMerge::cleanup
        return true
    elsif $serve then
        if ENV['KPSEMETHOD'] && ENV['KPSEPORT'] then
            # # kpse_merge_done: require 'base/kpseremote'
            begin
                KpseRemote::start_server
            rescue
                return false
            else
                return true
            end
        else
            usage
            puts("")
            puts("message  : set 'KPSEMETHOD' and 'KPSEPORT' variables")
            return false
        end
    elsif $help || ! $filename || $filename.empty? then
        usage
        loadtree($tree)
        loadenvironment($environment)
        show_environment()
        return true
    elsif $batch && $filename && ! $filename.empty? then
        # todo, take commands from file and avoid multiple starts and checks
        return false
    else
        report("texmfstart version #{$version}")
        loadtree($tree)
        loadenvironment($environment)
        show_environment()
        if $make || $remove then
            if $filename == 'all' then
                makelist = $makelist
            else
                makelist = [$filename]
            end
            makelist.each do |filename|
                if $windows then
                    make(filename,true,false,$remove)
                elsif $unix then
                    make(filename,false,true,$remove)
                else
                    make(filename,$mswindows,!$mswindows,$remove)
                end
            end
            return true # guess
        elsif $browser && $filename =~ /^http\:\/\// then
            return launch($filename)
        else
            begin
                process do
                    if $direct || $filename =~ /^bin\:/ then
                        return direct($filename)
                    elsif $edit && ! $editor.empty? then
                        return edit($filename)
                    else # script: or no prefix
                        command = find(shortpathname($filename),$program)
                        if command then
                            register("THREAD",File.dirname(File.expand_path(command)))
                            return run(command)
                        else
                            report('unable to locate program')
                            return false
                        end
                    end
                end
            rescue
                report('fatal error in starting process')
                return false
            end
        end
    end

end

if execute(ARGV) then
    report("\nexecution was successful") if $verbose
    exit(0)
else
    report("\nexecution failed") if $verbose
    exit(1)
end

# exit (if ($?.to_i rescue 0) > 0 then 1 else 0 end)
