#!/usr/bin/env python3
r'''
------------------------------------------------------------------------------
    webquiz | Online quizzes generated from LaTeX using python and TeX4ht
               | This module mainly deals with command-line options and
               | settings and then calls MakeWebQuiz to build the quiz
------------------------------------------------------------------------------
    Copyright (C) Andrew Mathas and Donald Taylor, University of Sydney

    Distributed under the terms of the GNU General Public License (GPL)
                  http://www.gnu.org/licenses/

    This file is part of the WebQuiz system.

    <Andrew.Mathas@sydney.edu.au>
------------------------------------------------------------------------------
'''

import argparse
import codecs
import errno
import glob
import os
import re
import shutil
import signal
import subprocess
import sys

# imports of webquiz code
import webquiz_makequiz
import webquiz_templates
import webquiz_util

#################################################################################
# read in basic meta data such as author, version, ... and set debugging=False
try:
    metadata = webquiz_util.MetaData(webquiz_util.kpsewhich('webquiz.ini'), debugging=False)
except subprocess.CalledProcessError:
    # check to see if we are running from the zip file
    ini_file = os.path.join(webquiz_util.webquiz_file(''), '..', 'latex', 'webquiz.ini')
    try:
        metadata = webquiz_util.MetaData(ini_file, debugging=False)
    except (FileNotFoundError, subprocess.CalledProcessError):
        print(webquiz_templates.missing_webquiz_ini.format(ini_file))
        sys.exit(1)

# ---------------------------------------------------------------------------------------
def graceful_exit(sig, frame):
    ''' exit gracefully on SIGINT and SIGTERM'''
    if metadata:
        webquiz_util.webquiz_error(False, 'program terminated (signal {}\n  {})'.format(sig, frame))
    else:
        webquiz_util.webquiz_error(False, 'program terminated (signal {})'.format(sig))

signal.signal(signal.SIGINT, graceful_exit)
signal.signal(signal.SIGTERM, graceful_exit)


#################################################################################
def preprocess_with_pst2pdf(options, quiz_file):
    r'''
    Preprocess the latex file using pst2pdf. As we are preprocessing the file it
    is not enough to have latex pass us a flag that tells us to use pst2pdf.
    Instead, we have to extract the class file option from the tex file

    INPUT: quiz_file should be the name of the quiz file, WITHOUT the .tex extension
    '''
    options.talk('Preprocessing {} with pst2pdsf'.format(quiz_file))
    try:
        # pst2pdf converts pspicture environments to svg images and makes a
        # new latex file quiz_file+'-pdf' that includes these
        cmd = 'pst2pdf --svg --imgdir={q_file} {q_file}.tex'.format(q_file=quiz_file)
        # pst2pdf is missing a #!-header so we need shell=True
        options.run(cmd, shell=True)
    except OSError as err:
        if err.errno == errno.ENOENT:
            webquiz_util.webquiz_error(options.debugging, 'pst2pdf not found. You need to install pst2pdf to use the pst2pdf option', err)
        else:
            webquiz_util.webquiz_error(options.debugging, 'error running pst2pdf on {}'.format(quiz_file), err)

    # match \includegraphics commands
    fix_svg = re.compile(r'(\\includegraphics\[scale=1\])\{('+quiz_file+r'-fig-[0-9]*)\}')
    # the svg images are in the quiz_file subdirectory but latex can't
    # find them so we update the tex file to look in the right place
    try:
        with codecs.open(quiz_file + '-pdf.tex', 'r', encoding='utf8') as pst_file:
            with codecs.open(quiz_file + '-pdf-fixed.tex', 'w', encoding='utf8') as pst_fixed:
                for line in pst_file:
                    pst_fixed.write(fix_svg.sub(r'\1{%s/\2.svg}' % quiz_file, line))
    except OSError as err:
        webquiz_util.webquiz_error(options.debugging,
            'there was an problem running pst2pdf for {}'.format(quiz_file),
            err
        )

class WebQuizSettings:
    r'''
    Class for initialising webquiz. This covers both reading and writing the
    webquizrc file and copying files into the web directories during
    initialisation. The settings themselves are stored in the attribute
    settings, which is a dictionary. The class reads and writes the settings to
    the webquizrc file and the values of the settings are available as items:
        >>> wq = WebQuizSettings()
        >>> wq['webquiz_url']
        ... /WebQuiz
        >>> wq['webquiz_url'] = '/new_url'
    '''

    # default of settings for the webquizrc file - a dictionary of dictionaries
    # the 'help' field is for printing descriptions of the settings to help the
    # user - they are also printed in the webquizrc file
    settings = dict(
        webquiz_url={
            'default': '',
            'advanced': False,
            'help': 'Relative URL for the webquiz web directory',
        },
        webquiz_www={
            'default': '',
            'advanced': False,
            'help': 'Full path to WebQuiz web directory',
        },
        language={
            'default': 'english',
            'advanced': False,
            'help': 'Default language used on web pages'
        },
        engine = {
            'default': 'latex',
            'advanced': False,
            'help': 'Default TeX engine used to compile web pages',
            'values': dict(latex='', lua='--lua', xelatex='--xetex')
        },
        theme={
            'default': 'default',
            'advanced': False,
            'help': 'Default colour theme used on web pages'
        },
        breadcrumbs={
            'default': '',
            'advanced': False,
            'help': 'Breadcrumbs at the top of quiz page',
        },
        department={
            'default': '',
            'advanced': False,
            'help': 'Name of department',
        },
        department_url={
            'default': '/',
            'advanced': False,
            'help': 'URL for department',
        },
        institution={
            'default': '',
            'advanced': False,
            'help': 'Institution or university',
        },
        institution_url={
            'default': '/',
            'advanced': False,
            'help': 'URL for institution or university',
        },
        hide_side_menu={
            'default': 'false',
            'advanced': False,
            'help': 'Do not display the side menu at start of quiz',
        },
        one_page={
            'default': 'false',
            'advanced': False,
            'help': 'Display questions on one page',
        },
        random_order={
            'default': 'false',
            'advanced': False,
            'help': 'Randomly order the quiz questions',
        },
        webquiz_layout={
            'default': 'webquiz_layout',
            'advanced': True,
            'help': 'Name of python module that formats the quizzes',
        },
        make4ht={
            'default': '',
            'advanced': True,
            'help': 'Build file for make4ht',
        },
        mathjax={
            'default':
            'https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.1/MathJax.js',
            'advanced':
            True,
            'help':
            'URL for mathjax',
        },
        version={
            'advanced': False,
            'help': 'WebQuiz version number for webquizrc settings',
        })

    # by default we assume we don't need to print a initialisation warning
    initialise_warning = ''

    # turn debugging on by default because any error message that we hit before
    # we process the command line options really should not happen
    debugging = True

    # keep track of whether we have initialised
    have_initialised = False

    def __init__(self):
        '''
        First read the system webquizrc file and then read the
        to use some system settings and to override others.

        By default, there is no webquiz initialisation file. We first
        look for webquizrc in the webquiz source directory and then
        for .webquizrc file in the users home directory.
        '''
        self.settings['version']['default'] = metadata.version
        for key in self.settings:
            self.settings[key]['value'] = self.settings[key]['default']
            if not 'editable' in self.settings[key]:
                self.settings[key]['editable'] = False

        # define user and system rc file and load the ones that exist

        TEXMFLOCAL=''
        try:
            TEXMFLOCAL = webquiz_util.kpsewhich('-var-value TEXMFLOCAL')
        except subprocess.CalledProcessError:
            pass

        if TEXMFLOCAL == '':
            TEXMFLOCAL = webquiz_util.kpsewhich('-var-value TEXMFMAIN')

        self.system_rcfile = os.path.join(TEXMFLOCAL, 'tex', 'latex', 'webquiz', 'webquizrc')
        self.read_webquizrc(self.system_rcfile)

        # the user rc file defaults to:
        #   ~/.dotfiles/config/webquizrc if .dotfiles/config exists
        #   ~/.config/webquizrc if .config exists
        # and otherwise to ~/.webquizrc
        if os.path.isdir(os.path.join(os.path.expanduser('~'), '.dotfiles', 'config')):
            self.user_rcfile = os.path.join(os.path.expanduser('~'), '.dotfiles', 'config', 'webquizrc')
        elif os.path.isdir(os.path.join(os.path.expanduser('~'), '.config')):
            self.user_rcfile = os.path.join(os.path.expanduser('~'), '.config', 'webquizrc')
        else:
            self.user_rcfile = os.path.join(os.path.expanduser('~'), '.webquizrc')

        self.read_webquizrc(self.user_rcfile)

    def webquiz_debug(self, msg):
        r'''
            Customised debugging message for the MakeSettings module
        '''
        webquiz_util.webquiz_debug(self.debugging, 'main: '+msg)

    def webquiz_error(self, msg, err=None):
        r'''
            Customised error messages for the Module
        '''
        webquiz_util.webquiz_error(self.debugging, 'settings: '+msg, err)

    def __getitem__(self, key):
        r'''
        Return the value of the corresponding setting. That is, it returns
            self.settings[key]['value']
        and an error if the key is unknown.
        '''
        if key in self.settings:
            return self.settings[key]['value']

        self.webquiz_error('getitem: unknown setting "{}" in webquizrc.'.format(key))

    def __setitem__(self, key, value):
        r'''
        Set the value of the corresponding setting. This is the equivalent of
            self.settings[key]['value'] = value
        and an error if the key is unknown.
        '''
        if key in self.settings:
            self.settings[key]['value'] = value
        else:
            self.webquiz_error('setitem: unknown setting "{}" in webquizrc'.format(key))

    def read_webquizrc(self, rcfile, must_exist=False):
        r'''
        Read the settings from the specified webquizrc file - if it exists, in
        which case set self.rcfile equal to this directory. If the file does
        not exist then return without changing the current settings.
        '''
        if os.path.isfile(rcfile):
            try:
                with codecs.open(rcfile, 'r', encoding='utf8') as webquizrc:
                    for line in webquizrc:
                        if '#' in line:  # remove comments
                            line = line[:line.index('#')]
                        if '=' in line:
                            key, value = line.split('=')
                            key = key.strip().lower().replace('-','_')
                            value = value.strip()
                            if key in self.settings:
                                if value != self[key]:
                                    self[key] = value
                            elif key != '':
                                self.webquiz_error('unknown setting "{}" in {}'.format(key, rcfile))

                # record the rcfile for later use
                self.rcfile = rcfile

            except OSError as err:
                self.webquiz_error('there was a problem reading the rc-file {}'.format(rcfile), err)

            except Exception as err:
                self.webquiz_error('there was an error reading the webquizrc file,', err)

        elif must_exist:
            # this is only an error if we have been asked to read this file
            self.webquiz_error('the rc-file "{}" does not exist'.format(rcfile))

    def keys(self):
        r'''
        Return a list of keys for all settings, ordered alphabetically with the
        advanced options last/
        '''
        return sorted(self.settings.keys(), key=lambda k: '{}{}'.format(self.settings[k]['advanced'], k))

    def write_webquizrc(self):
        r'''
        Write the settings to the webquizrc file, defaulting to the user
        rcfile if unable to write to the system rcfile
        '''
        if not hasattr(self, 'rcfile'):
            # when initialising an rcfile will not exist yet
            self.rcfile = self.system_rcfile

        file_not_written = True
        while file_not_written:
            try:
                dire = os.path.dirname(self.rcfile)
                if dire != '' and not os.path.isdir(dire):
                    os.makedirs(dire, exist_ok=True)
                with codecs.open(self.rcfile, 'w', encoding='utf8') as rcfile:
                    for key in self.keys():
                        # Only save settings in the rcfile if they have changed
                        # Note that changed means changed from the last read
                        # rcfile rather than from the default (of course, the
                        # defaults serve as the "initial rcfile")
                        if key == 'version' or self.settings[key]['default']!=self[key]:
                            rcfile.write('# {}\n{:<17} = {}\n'.format(
                                           self.settings[key]['help'],
                                           key.replace('_','-'),
                                           self[key])
                            )

                print('\nWebQuiz settings saved in {}\n'.format( self.rcfile))
                input('Press RETURN to continue... ')
                file_not_written = False

            except (OSError, PermissionError) as err:
                # if writing to the system_rcfile then try to write to user_rcfile
                alt_rcfile = self.user_rcfile if self.rcfile != self.user_rcfile else self.system_rcfile
                response = input(
                    webquiz_templates.rc_permission_error.format(
                        error=err,
                        rcfile=self.rcfile,
                        alt_rcfile=alt_rcfile))
                if response.startswith('2'):
                    self.rcfile = alt_rcfile
                elif response.startswith('3'):
                    rcfile = input('WebQuiz rc-file: ')
                    print('\nTo access this rc-file you will need to use: webquiz --rcfile {} ...'.format(rcfile))
                    self.rcfile = os.path.expanduser(rcfile)
                elif not response.startswith('1'):
                    print('exiting...')
                    sys.exit(1)

    def list_settings(self, setting='all'):
        r'''
        Print the non-default settings for webquiz from the webquizrc
        '''
        if not hasattr(self, 'rcfile'):
            print(webquiz_templates.initialise_settings)
            sys.exit(1)

        if setting not in ['all', 'verbose', 'help']:
            setting = setting.replace('-', '_')
            if setting in self.settings:
                print(self.settings[setting]['value'])
            else:
                self.webquiz_error('{} is an invalid setting'.format(setting))

        elif setting=='all':
            dash = '-'*len('WebQuiz rc-file: {}'.format(self.rcfile))
            print('{dash}\nWebQuiz rc-file: {rcfile}\n{dash}'.format(rcfile=self.rcfile, dash=dash))
            for key in self.keys():
                print('{:<17} = {}'.format(key.replace('_', '-'), self[key]))
            print('{dash}'.format(dash=dash))

        elif setting=='help':
            for key in self.keys():
                print('{:<17} {}'.format(key.replace('_', '-'), self.settings[key]['help'].lower()))

        else:
            print('WebQuiz settings from {}'.format(self.rcfile))
            for key in self.keys():
                print('# {}{}\n{:<17} = {:<17}  {}'.format(
                        self.settings[key]['help'],
                        ' (advanced)' if self.settings[key]['advanced'] else '',
                        key.replace('_', '-'),
                        self[key],
                        '(default)' if self[key]==self.settings[key]['default'] else ''
                        )
                )

    def initialise_webquiz(self, need_to_initialise=False, developer=False):
        r'''
        Set the root for the WebQuiz web directory and copy the www files into
        this directory. Once this is done save the settings to webquizrc.
        This method should only be used when WebQuiz is being set up.

        If `need_to_initialise` is `True` then this is a forced initialisation.
        '''

        # keep track of whether we have initialised
        self.have_initialised = True

        if need_to_initialise:
            self.initialise_warning = webquiz_templates.web_initialise_warning
            initialise = input(webquiz_templates.initialise_invite)
            if initialise!='' and initialise.strip().lower()[0]!='y':
                self['webquiz_url'] = 'http://www.maths.usyd.edu.au/u/mathas/WebQuiz'
                return

        if self['webquiz_url']=='':
            self['webquiz_url'] = '/WebQuiz'

        # prompt for directory and copy files - are these reasonable defaults
        # for each OS?
        if sys.platform == 'darwin':
            default_root = '/Library/WebServer/Documents/WebQuiz'
            platform = 'Mac OSX'
        elif sys.platform.startswith('win'):
            default_root = 'c:\inetpub\wwwroot\WebQuiz'
            platform = 'Windows'
        else:
            default_root = '/var/www/html/WebQuiz'
            platform = sys.platform.capitalize()

        if self['webquiz_www'] != '':
            webquiz_root = self['webquiz_www']
        else:
            webquiz_root = default_root

        print(webquiz_templates.initialise_introduction)
        input('Press RETURN to continue... ')

        print(webquiz_templates.webroot_request.format(
                platform=platform,
                webquiz_dir = webquiz_root)
        )
        input('Press RETURN to continue... ')

        files_copied = False
        while not files_copied:
            web_dir = input('\nWebQuiz web directory:\n[{}] '.format(webquiz_root))
            if web_dir == '':
                web_dir = webquiz_root
            else:
                web_dir = os.path.expanduser(web_dir)

            print('Web directory set to {}'.format(web_dir))
            if web_dir=='SMS':
                # undocumented: allow links to SMS web pages
                self['webquiz_www'] = 'SMS'
                self['webquiz_url'] = 'http://www.maths.usyd.edu.au/u/mathas/WebQuiz'

            else:
                try:
                    # ...remove the doc directory
                    web_doc = os.path.join(web_dir, 'doc')
                    if os.path.isfile(web_doc) or os.path.islink(web_doc):
                        os.remove(web_doc)
                    elif os.path.isdir(web_doc):
                        shutil.rmtree(web_doc)

                    # Need to locate the www directory, which should be a subdirectory
                    # of the webquiz doc directory. First try using texdoc
                    webquiz_doc = ''
                    try:
                        webquiz_pdf = webquiz_util.shell_command('texdoc --list --machine webquiz.pdf').split()[-1]
                        if webquiz_pdf.endswith('webquiz.pdf'):
                            webquiz_doc = os.path.dirname(webquiz_pdf)
                    except subprocess.CalledProcessError:
                        pass

                    # if texdoc failed then try using TEXMFMAIN
                    if webquiz_doc=='':
                        try:
                            webquiz_doc = os.path.join(webquiz_util.kpsewhich('-var-value TEXMFMAIN'), 'doc','latex', 'webquiz')
                        except subprocess.CalledProcessError:
                            pass

                    # if we still don't have webquiz_doc then try working backwards from webquiz.cls
                    # unlikely to work if TEXMFMAIN doesn't
                    if not os.path.isdir(webquiz_doc):
                        parent = os.path.dirname
                        try:
                            texdist_dir = parent(parent(parent(parent(parent(webquiz_util.kpsewhich('webquiz.cls'))))))
                        except subprocess.CalledProcessError:
                            print(webquiz_templates.not_installed.format(metadata.repository))
                            sys.exit(1)

                        webquiz_doc = os.path.join(texdist_dir, 'doc', 'latex', 'webquiz')

                    # get the root directory of the source code for developer
                    # mode and just in case webquiz_www still does not exist
                    webquiz_src = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))

                    webquiz_www = os.path.join(webquiz_doc, 'www')
                    if not os.path.isdir(webquiz_www):
                        webquiz_www = os.path.join(webquiz_src, 'doc', 'www')

                    if developer and os.path.isdir(os.path.join(webquiz_src, 'doc')):
                        # this is a development version so add links from the
                        # web directory to the css,doc and js directories
                        print('\nInstalling files for development version')
                        print('Linking web files {} -> {} ...\n'.format(web_dir, webquiz_src))
                        if not os.path.exists(web_dir):
                            os.makedirs(web_dir)

                        for (src, target) in [('javascript', 'js'), ('css', 'css'), ('doc', 'doc')]:
                            newlink = os.path.join(web_dir, target)
                            try:
                                os.remove(newlink)
                            except FileNotFoundError:
                                pass
                            try:
                                os.symlink(os.path.join(webquiz_src,src), newlink)
                            except OSError as err:
                                print('There was a problem linking {}: {}'.format(newlink, err))

                    else:
                        # loop until we find some files to install or exit
                        while not os.path.isdir(webquiz_www) or webquiz_www=='':
                            print('\nUnable to find the WebQuiz web files')
                            webquiz_www = input('Please enter the location of the WebQuiz www directory\nor press RETURN to exit: ')
                            webquiz_www = os.path.expanduser(webquiz_www)
                            if webquiz_www=='':
                                sys.exit()
                            if not (webquiz_www.endswith('www/') or webquiz_www.endswith('www')):
                                print('\nThe webquiz web directory is called www, so\n  {}\ncannot be the right directory.'.format(
                                      webquiz_www)
                                )
                                webquiz_www = False


                        # the www directory exists so we copy it to web_dir
                        print('\nCopying web files to {} ...'.format(web_dir))
                        webquiz_util.copytree(webquiz_www, web_dir)

                    self['webquiz_www'] = web_dir
                    files_copied = True

                except PermissionError:
                    print(webquiz_templates.permission_error.format(web_dir))

                except OSError as err:
                    print(webquiz_templates.oserror_copying.format(web_dir=web_dir, err=err))

        if self['webquiz_www']!='SMS':
            # now prompt for the relative url
            webquiz_url = input(webquiz_templates.webquiz_url_message.format(self['webquiz_url']))
            if webquiz_url != '':
                # removing trailing slashes from webquiz_url
                while webquiz_url[-1] == '/':
                    webquiz_url = webquiz_url[:len(webquiz_url) - 1]

                if webquiz_url[0] != '/':  # force URL to start with /
                    webquiz_url = '/' + webquiz_url

                if not web_dir.endswith(webquiz_url):
                    print(webquiz_templates.webquiz_url_warning)
                    input('Press RETURN to continue... ')

                self['webquiz_url'] = webquiz_url

        # save the settings and exit
        self.write_webquizrc()
        print(webquiz_templates.initialise_ending.format(web_dir=self['webquiz_www']))

    def edit_settings(self):
        r'''
        Change current default values for the WebQuiz settings
        '''
        advanced_not_started = True
        for key in self.keys():
            if key not in ['webquiz_www', 'version']:
                if advanced_not_started and self.settings[key]['advanced']:
                    print(webquiz_templates.advanced_settings)
                    advanced_not_started = False

                skey = '{}'.format(self[key])
                setting = input('{}{}[{}]: '.format(
                                    self.settings[key]['help'],
                                    ' ' if len(skey)<40 else '\n',
                                    skey
                          )
                ).strip()
                if setting != '':
                    if key == 'webquiz_url' and setting[0] != '/':
                        print("  ** prepending '/' to webquiz_url **")
                        setting = '/' + setting

                    elif key == 'webquiz_layout':
                        setting = os.path.expanduser(setting)
                        if setting.endswith('.py'):
                            print("  ** removing .py extension from webquiz_layout **")
                            setting = setting[:-3]

                    elif key == 'engine' and setting not in self.settings['engine'].values:
                        print('setting not changed: {} is not a valid TeX engine'.format(setting))
                        setting = self['engine']

                    elif key in ['hide_side_menu', 'random_order']:
                        setting = setting.lower()
                        if setting not in ['true', 'false']:
                            print('setting not changed: {} must be True or False'.format(key))
                            setting = self[key]

                    elif setting=='NONE':
                        setting = ''

                    self[key] = setting

        # save the settings, print them and exit
        self.write_webquizrc()
        self.list_settings()

    def tex_install(self):
        r'''
        Install the tex files into the standard locations in TEXMFMAIN:
            scripts -> TEXMFMAIN/scripts/webquiz
            doc     -> TEXMFMAIN/doc/latex/webquiz
            latex   -> TEXMFMAIN/tex/latex/webquiz
        It is assumed that this is run from the zipfile installation. There
        is little in the way of error checking or debugging.

        Undocumented feature - useful for debugging initialisation routine
        '''
        webquiz_top = os.path.abspath(webquiz_util.webquiz_file('..'))
        texmf = webquiz_util.kpsewhich('-var-value TEXMFMAIN')
        for (src, target) in [('scripts', 'scripts'),
                         ('latex', 'tex/latex'),
                         ('doc', 'doc/latex')]:
            try:
                webquiz_util.copytree(os.path.join(webquiz_top,src), os.path.join(texmf, target, 'webquiz'))

            except (FileExistsError,FileNotFoundError):
                continue

            except PermissionError as err:
                print(webquiz_templates.insufficient_permissions.format(err))
                sys.exit(1)

        try:

            # add a link to webquiz.py
            texbin = os.path.dirname(shutil.which('pdflatex'))
            if sys.platform.startswith('win'):
                shutil.copyfile(
                    os.path.join(texmf,'scripts','webquiz','webquiz.bat'), 
                    os.path.join(texbin, 'webquiz.bat')
                )
            else:
                os.symlink(
                    os.path.join(texmf,'scripts','webquiz','webquiz.py'), 
                    os.path.join(texbin, 'webquiz')
                )
            subprocess.call('mktexlsr', shell=True)

        except (FileExistsError,FileNotFoundError):
            pass

        except PermissionError as err:
            print(webquiz_templates.insufficient_permissions.format(err))
            sys.exit(1)

        except subprocess.CalledProcessError as err:
            self.webquiz_error('There was a problem running mktexlsr', err)

    def tex_uninstall(self):
        r'''
        UnInstall the tex files into TEXMFMAIN. It is assumed that the files
        are installed in the natural locations in the TEXMFMAIN tree, namely:
            scripts -> TEXMFMAIN/scripts/webquiz
            doc     -> TEXMFMAIN/doc/latex/webquiz
            latex   -> TEXMFMAIN/tex/latex/webquiz
        There is little in the way of error checking or debugging.

        Undocumented feature - useful for debugging initialisation routine
        '''
        webquiz_top = os.path.abspath(webquiz_util.webquiz_file('..'))
        texmf = webquiz_util.kpsewhich('-var-value TEXMFMAIN')
        for target in ['scripts', 'tex/latex', 'doc/latex']:
            try:
                shutil.rmtree(os.path.join(texmf, target, 'webquiz'))

            except (FileExistsError,FileNotFoundError):
                pass

            except PermissionError as err:
                print(webquiz_templates.insufficient_permissions.format(err))
                sys.exit(1)

        try:
            # remove link from texbin to webquiz.py
            texbin = os.path.dirname(shutil.which('pdflatex'))
            if sys.platform.startswith('win'):
                os.remove(os.path.join(texbin, 'webquiz.bat'))
            else:
                os.remove(os.path.join(texbin, 'webquiz'))

        except (FileExistsError,FileNotFoundError):
            pass

        except PermissionError as err:
            print(webquiz_templates.insufficient_permissions.format(err))
            sys.exit(1)

        # remove any rcfiles that exist in obvious places
        try:
            if os.path.isfile(self.system_rcfile):
                os.remove(self.system_rcfile)
            if os.path.isfile(self.user_rcfile):
                os.remove(self.user_rcfile)
            if os.path.isfile(self.rcfile):
                os.remove(self.rcfile)
        except PermissionError:
            print(webquiz_templates.insufficient_permissions.format(err))
            sys.exit(1)

        # remove link to webquiz.py
        texbin = os.path.dirname(shutil.which('pdflatex'))
        try:
            if sys.platform.startswith('win'):
                webquiz = os.path.join(texbin, 'webquiz.bat')
                os.remove(webquiz)
            else:
                webquiz = os.path.join(texbin,'webquiz')
                target = os.readlink(webquiz)
                if target==os.path.join(texmf,'scripts','webquiz','webquiz.py'):
                    os.remove(webquiz)

        except (FileExistsError,FileNotFoundError):
            pass

        except OSError as err:
            print('There was a problem removing the link to webquiz: {}'.format(err))

    def uninstall_webquiz(self):
        r'''
        Remove all of the webquiz files from the webserver
        '''

        if os.path.isdir(self['webquiz_www']):
            remove = input('Do you really want to remove the WebQuiz from your web server [N/yes]? ')
            if remove != 'yes':
                print('WebQuiz unistall aborted!')
                return

            try:
                shutil.rmtree(self['webquiz_www'])
                print('WebQuiz files successfully removed from {}'.format(self['webquiz_www']))

            except PermissionError as err:
                print(webquiz_templates.insufficient_permissions.format(err))
                sys.exit(1)

            except OSError as err:
                self.webquiz_error('There was a problem removing webquiz files from {}'.format(self['webquiz_www']), err)

            # now reset and save the locations of the webquiz files and URL
            self['webquiz_url'] = ''
            self['webquiz_www'] = ''
            self.write_webquizrc()

        else:
            self.webquiz_error('uninstall: no webwquiz files are installed on your web server??')

        for rfile in ['system', 'user']:
            rcfile = getattr(self, rfile+'_rcfile')
            if os.path.isfile(rcfile):
                rm = input('Remove {} rcfile {}\n[Y/no] '.format(rfile, rcfile))
                if rm != 'no':
                    try:
                        os.remove(rcfile)
                    except (OSError, PermissionError) as err:
                        self.webquiz_error('There was a problem deleting {}'.format(rcfile), err)


# =====================================================
if __name__ == '__main__':
    try:
        settings = WebQuizSettings()

        # parse the command line options
        parser = argparse.ArgumentParser(description=metadata.description)

        parser.add_argument(
            'quiz_file',
            nargs='*',
            type=str,
            default=None,
            help='latex quiz files')

        parser.add_argument(
            '-q',
            '--quiet',
            action='count',
            default=0,
            help='Suppress tex4ht messages (also -qq etc)')

        parser.add_argument(
            '-d', '--draft',
            action='store_true',
            default=False,
            help='Use make4ht draft mode')

        parser.add_argument(
            '-s',
            '--shell-escape',
            action='store_true',
            default=False,
            help='Shell escape for tex4ht/make4ht')

        engine = parser.add_mutually_exclusive_group()
        engine.add_argument(
            '--latex',
            action='store_const',
            const='latex',
            default=settings['engine'],
            dest='engine',
            help='Use latex to compile document with make4ht (default)')
        engine.add_argument(
            '-l',
            '--lua',
            action='store_const',
            const='lua',
            dest='engine',
            help='Use lualatex to compile the quiz')
        engine.add_argument(
            '-x',
            '--xelatex',
            action='store_const',
            const='xelatex',
            dest='engine',
            help='Use xelatex to compile the quiz')

        parser.add_argument(
            '-r',
            '--rcfile',
            action='store',
            default=None,
            help='Specify location of the webquiz rc-file ')

        settings_parser = parser.add_mutually_exclusive_group()
        settings_parser.add_argument(
            '-i',
            '--initialise',
            '--initialize',
            action='store_true',
            default=False,
            help='Install web components of webquiz')
        settings_parser.add_argument(
            '-e', '--edit-settings',
            action='store_true',
            default=False,
            help='Edit default settings for webquiz')
        settings_parser.add_argument(
            '--settings',
            action='store',
            const='all',
            default='',
            nargs='?',
            type=str,
            help='List default settings for webquiz'
        )
        settings_parser.add_argument(
            '--developer',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS
        )

        # options suppressed from the help message
        parser.add_argument(
            '-m',
            '--make4ht',
            action='store',
            type=str,
            dest='make4ht_options',
            default=settings['make4ht'],
            help=argparse.SUPPRESS
        )

        parser.add_argument(
            '--webquiz_layout',
            action='store',
            type=str,
            dest='webquiz_layout',
            default=settings['webquiz_layout'],
            help=argparse.SUPPRESS
        )

        install_parser = parser.add_mutually_exclusive_group()
        install_parser.add_argument(
            '--tex-install',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS
        )
        install_parser.add_argument(
            '--tex-uninstall',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS
        )
        install_parser.add_argument(
            '--uninstall',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS
        )

        parser.add_argument(
            '--version',
            action='version',
            version='%(prog)s version {}'.format(metadata.version),
            help=argparse.SUPPRESS)

        parser.add_argument(
            '--debugging',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS)

        parser.add_argument(
            '--shorthelp',
            action='store_true',
            default=False,
            help=argparse.SUPPRESS
        )

        # parse the options
        options = parser.parse_args()
        options.prog = parser.prog

        # set debugging mode from options
        settings.debugging = options.debugging

        # read the rcfile and throw an error if we are not adjusting the settings
        if options.rcfile is not None:
            rcfile = os.path.expanduser(options.rcfile)
            settings.read_webquizrc(rcfile)

        if options.uninstall:
            # uninstall web files and exit
            settings.uninstall_webquiz()
            sys.exit()
        elif options.tex_install:
            # install files from zip file into tex distribution and then exit
            settings.tex_install()
            sys.exit()
        elif options.tex_uninstall:
            # install files from zip file into tex distribution and then exit
            settings.tex_uninstall()
            sys.exit()

        # initialise and exit
        if options.initialise or options.developer:
            settings.initialise_webquiz(developer=options.developer)

        # force initialisation if the url is not set
        elif settings['webquiz_url'] == '':
            settings.initialise_webquiz(need_to_initialise=True)

        # list settings and exit
        if options.settings != '':
            settings.list_settings(options.settings)
            sys.exit()

        # edit settings and exit
        if options.edit_settings:
            settings.edit_settings()
            sys.exit()

        # print short help and exit
        if options.shorthelp:
            parser.print_usage()
            sys.exit()

        # if no filename then exit
        if options.quiz_file==[]:
            if settings.have_initialised:
                sys.exit()
            else:
                parser.print_help()
                sys.exit(1)

        # import the local page formatter
        mod_dir, mod_layout = os.path.split(options.webquiz_layout)
        if mod_dir != '':
            sys.path.insert(0, mod_dir)
        options.write_web_page = __import__(mod_layout).write_web_page

        # run() is a shorthand for executing system commands depending on the quietness
        #       - we need to use shell=True because otherwise pst2pdf gives an error
        # options.talk() is a shorthand for letting the user know what is happening
        if options.quiet == 0:
            options.run = webquiz_util.run
            options.talk = lambda msg: print(msg)
        elif options.quiet == 1:
            options.run  = webquiz_util.quiet_run
            options.talk = lambda msg: print(msg)
        else:
            options.run  = webquiz_util.silent_run
            options.talk = lambda msg: None

        # run through the list of quizzes and make them
        for quiz_file in options.quiz_file:
            if len(options.quiz_file) > 1 and options.quiet < 3:
                print('Making web page for {}'.format(quiz_file))

            quiz_name, ext = os.path.splitext(quiz_file)  # extract filename and extension
            # quiz_file is assumed to be a tex file if no extension is given
            if ext=='':
                ext = '.tex'
                quiz_file += ext
 
            # windows likes adding a prefix of '.\\'to filename and this causes havoc with latex
            if os.path.dirname(quiz_file)=='.':
                quiz_file = os.path.basename(quiz_file)

            if ext not in ['.tex', '.xml']:
                    webquiz_util.webquiz_error(True, 'unrecognised file extension {}'.format(ext))

            if not os.path.isfile(quiz_file):
                webquiz_util.webquiz+error(True, 'WebQuiz error: cannot read file {}'.format(quiz_file))

            # the quiz name and the quiz_file will be different if pst2pdf is used
            quiz_name = quiz_file
            if options.quiet < 2:
                print('WebQuiz generating web page for {}'.format(quiz_file))

            options.pst2pdf = False
            if ext==".tex":
                # If the pst2pdf option is used then we need to preprocess
                # the latex file BEFORE passing it to MakeWebQuiz. Set
                # options.pst2pdf = True if pst2pdf is given as an option to
                # the webquiz documentclass
                with codecs.open(quiz_file, 'r', encoding='utf8') as q_file:
                    doc = q_file.read()

                try:
                    brac = doc.index(r'\documentclass[') + 15  # start of class options
                    if 'pst2pdf' in [
                            opt.strip()
                            for opt in doc[brac:brac+doc[brac:].index(']')].split(',')
                    ]:
                        preprocess_with_pst2pdf(options, quiz_file[:-4])
                        options.pst2pdf = True
                        # now run webquiz on the modified tex file
                        quiz_file = quiz_file[:-4] + '-pdf-fixed.tex'
                except ValueError:
                    pass

            # the file exists and is readable so make the quiz
            webquiz_makequiz.MakeWebQuiz(quiz_name, quiz_file, options, settings, metadata)

            quiz_name, ext = os.path.splitext(quiz_name)  # extract filename and extension

            # move the css file into the directory for the quiz
            css_file = os.path.join(quiz_name, quiz_name + '.css')
            if os.path.isfile(quiz_name + '.css'):
                if os.path.isfile(css_file):
                    os.remove(css_file)
                shutil.move(quiz_name + '.css', css_file)

            # now clean up unless debugging
            if not options.debugging:
                for ext in ['4ct', '4tc', 'dvi', 'idv', 'lg', 'log',
                    'ps', 'pdf', 'tmp', 'xml', 'xref'
                ]:
                    if os.path.isfile(quiz_name + '.' + ext):
                        os.remove(quiz_name + '.' + ext)

                # files created when using pst2pdf
                if options.pst2pdf:
                    for file in glob.glob(quiz_name + '-pdf.*'):
                        os.remove(file)
                    for file in glob.glob(quiz_name + '-pdf-fixed.*'):
                        os.remove(file)
                    for extention in ['.preamble', '.plog', '-tmp.tex',
                            '-pst.tex', '-fig.tex'
                    ]:
                        if os.path.isfile(quiz_name + extention):
                            os.remove(quiz_name + extention)
                    if os.path.isdir(os.path.join(quiz_name, quiz_name)):
                        shutil.rmtree(os.path.join(quiz_name, quiz_name))

        if settings.initialise_warning != '' and not settings.have_initialised:
            print(webquiz_templates.text_initialise_warning)

    except Exception as err:

        # there is a small chance that there is an error before the
        # settings.debugging flag has been set
        webquiz_util.webquiz_error(settings.debugging if 'settings' in globals() else True,
            'unknown problem.\n\nIf you think this is a bug please report it by creating an issue at\n    {}\n'
            .format(metadata.repository), err)
