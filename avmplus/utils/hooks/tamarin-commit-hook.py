#! /usr/bin/python
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/. 
#
#  Hook script used by tamarin team on tamarin-redux and tamarin-central.
#
# For documentation on hook scripts see:
#   http://hgbook.red-bean.com/read/handling-repository-events-with-hooks.html
#   http://mercurial.selenic.com/wiki/MercurialApi

# This file is to be run using a pretxncommit hook
# (and, at your option, a preoutgoing hook).
# Place this in your .hg/hgrc file in the repo:
#
# [hooks]
# pretxncommit.master = python:/path/to/tamarin-commit-hook.py:master_hook
# preoutgoing.checklog = python:/path/to/tamarin-commit-hook.py:preoutgoing_hook
#
# ====
#
# OVERVIEW
#
# These hooks check that the changesets you commit and push adhere to
# some fairly loose formatting and information-content rules.
#
# Ground rules:
# - Neither hook prevents the commit or push of any changeset (as long
#   as you respond affirmatively when prompted for confirmation).
# - Neither hook should significantly impede your personal workflow.
#   Mercurial Queues and other extensions should work without
#   introducing a new prompt in the common case*.
#
# Counter-examples to either of these two ground rules should be filed as
# bug against the hooks.  (The objective is to reduce pollution in the
# central repository, not impose a strait-jacket on your development
# style.)
#
#
# THE PRETXNCOMMIT HOOK
#
# The pretxncommit hook, named master_hook below, fires whenever the
# user runs a transaction to create a changeset in the working
# repository via a command such as 'hg commit' or 'hg import <patch>'.
# The master_hook checks that all changesets introduced by the
# transaction satisfy two rules:
#
# - Source code changes (for a standard set of languages including
#   C++, AS3, and Python) should have clean whitespace**; this means
#   they contain no new occurrences of: (1.) tab characters, (2.)
#   Microsoft Windows line-endings ('\r'), or (3.) trailing whitespace
#   on a line.
#
# - Source code changes should not contain the security change marker
#   used to demarcate code that is not meant for release to public
#   repositories.
#
# If any of the above checks fail, the user is prompted with a
# description of the failing checks and a request to confirm that they
# still want the transaction (i.e. commit or import) to proceed.
#
#
# THE PREOUTGOING HOOK
#
# The preoutgoing_hook fires whenever the user transfers changesets
# between repositories, via a command such a 'hg push' or 'hg pull'.
# The preoutgoing_hook performs a heuristic*** scan of a changeset to
# check:
# - the changeset's log message has a Bugzilla ticket number,
# - the associated Bugzilla ticket is not tagged as a security bug,
# - the changeset's log message has a parenthesized list of reviewers, and
# - the changeset's user has a validly formatted email address.
#
# If any of the above checks fail, the user is prompted with a
# description of the failing checks and a request to confirm that they
# still want the transfer (i.e. push or pull) to proceed.  One can
# obviously still push to security bugs (or push without reviews,
# without an associated ticket, etc); this is just meant as an extra
# hurdle providing a moment for the user to ask reflectively "Am I
# pushing to the right repository?  Am I doing the right thing?"
#
#
# FOOTNOTES
#
# * (Yes, "in the common case" is deliberate weasel wording, as is
#   "significantly impede".  For example, the hooks may prompt
#   occasionally, e.g. when pushing between your own private
#   repositories.)
#
# ** See utils/fixtabs for a cleap-up utility if you feel like fixing
#   a whole file rather than remove just your own violations of the
#   whitespace rules.
#
# *** The preoutgoing_hook's scan is considered heuristic because it
#   only scans the tip changeset.  It restricts its attention to the
#   tip (and not to other changesets that may be included in the push
#   or pull) for technical reasons documented on Bugzilla 630416 and
#   in the source below.  Scanning the tip alone is a sufficient
#   compromise because it captures the common case where the tip
#   changeset is the sole change being pushed to our central
#   repository.

import sys, re, os
from mercurial import hg, ui, commands, patch
from mercurial.node import hex, short
from HTMLParser import HTMLParser
from urllib2 import urlopen

try:
    # Mercurial 1.9
    from mercurial import scmutil
    matchfiles = scmutil.matchfiles
except ImportError:
    from mercurial import cmdutil
    matchfiles = cmdutil.matchfiles


class BugType:
    NORMAL = 1
    SECURITY = 2
    INVALID = 3

class TitleParser(HTMLParser):
    '''Very simple parser to extract the title from an HTML page'''
    in_title = False
    title = ''
    def handle_starttag(self, tag, attrs):
        if tag == 'title':
            self.in_title = True
    def handle_data(self, data):
        if self.in_title:
            self.title = data
            self.in_title = False

def master_hook(ui, repo, **kwargs):
    ui.debug('running tamarin master_hook\n')
    ui.debug('kwargs: %s\n' % kwargs)
    # The mercurial hook script expects the equivalent of an exit code back from
    # this call:
    #   False = 0 = No Error : allow push
    #   True = 1 = Error : abort push
    error = False
    error = error or diff_check(ui, repo, **kwargs)

    if error:
        # Save the commit message so it can be reused by user
        desc = repo[repo[kwargs['node']].rev()].description()
        ui.debug('Description: %s\n' % desc)
        try:
            f = open('%s/.hg/commit.save' % repo.root, 'w')
            f.write(desc)
            f.close()
            ui.warn('Commit message saved to .hg/commit.save\nSaved message can be recommitted using -l .hg/commit.save\n')
        except IOError:
            ui.warn('Error writing .hg/commit.save file')

    return error

def preoutgoing_hook(ui, repo, **kwargs):
    ui.debug('running tamarin preoutgoing_hook\n')
    ui.debug('kwargs: %s\n' % kwargs)
    operation = kwargs['source']

    # Like master_hook, return code False implies No Error, allow push.
    error = False
    error = error or heuristic_log_check(ui, repo, operation, **kwargs)

    return error

def heuristic_log_check(ui, repo, operation, **kwargs):
    # Bug 630416: Unlike master_hook, the hg preoutgoing hook (as of
    # Mercurial version 1.7) has very little to work with: no
    # reference to targeted repo, no description of changesets being
    # gathered to propagate, etc.
    #
    # We just want to catch log entry oversights before pushing to
    # other repositories.  As a heuristic, assume tip changeset is the
    # (only) revision being pushed; heuristic can misfire, but should
    # catch the common cases (a more formal guard would belong
    # server-side anyway).
    #
    # If future Mercurial versions address this problem with
    # preoutgoing, then could drop heuristic and apply description
    # check across all outgoing changesets; then we should print all
    # warnings in one pass and prompt for confirmation at most once.

    # Don't do format-check on hg-strip
    if (operation in ["strip"]):
        return False

    tip_id = repo.changelog.tip()
    tip_changeset = repo[tip_id]

    # See master_hook for interpretation of error flag.
    error = check_desc_for_bugnum_and_reviews(ui, tip_changeset, operation)
    error = error or check_user_for_valid_email(ui, tip_changeset, operation)
    return error

def prompt_yesno(ui, operation):
    return ui.promptchoice(('Continue %s (n)o, (y)es? [n]' % operation),
                           (('&No'),('&Yes')), 0)

def bugzilla_reference(line):
    # Match bug number of >= 6 digits and prefixed by "Bug", "For", etc
    try:
        bug_number = re.match(r'.*(Bug|For|Fix)\s*([0-9]{6,})',
                              line, re.IGNORECASE).group(2)
    except AttributeError:
        return None
    return bug_number

def has_reviewer_notes(line):
    # Match "r=<name>" or "r+<name>"; assumes names are alphanumeric.
    return re.match(r'.*r(=|\+)[a-zA-Z0-9]+', line)

def check_bug_type(bug):
    p = TitleParser()
    u = urlopen('https://bugzilla.mozilla.org/show_bug.cgi?id=%s' % bug)
    p.feed(u.read().decode(u.info().getparam('charset')))
    p.close()
    
    if p.title == 'Access Denied':
        return BugType.SECURITY
    elif p.title == 'Invalid Bug ID':
        return BugType.INVALID
    return BugType.NORMAL

def has_email_in_brackets(user):
    return re.match(r'.*<.+>', user)

def has_email_with_domain(user):
    return re.match(r'.*<.+@.+>', user)

def check_user_for_valid_email(ui, changeset, operation):
    user = changeset.user()
    ui.debug('\ncheck_user_for_valid_email: %s' % user)
    has_email = has_email_in_brackets(user)
    if not has_email:
        ui.warn('\nUser missing email address for changeset %s: \n  %s\n'
                % (changeset, user))
        response = prompt_yesno(ui, operation)
        if response == 0:
            ui.warn('Aborting %s due to user missing email.\n' % operation)
            return True;
    else:
        has_domain = has_email_with_domain(user)
        if not has_domain:
            ui.warn('\nUser email missing domain for changeset %s: \n  %s\n'
                    % (changeset, user))
            response = prompt_yesno(ui, operation)
            if response == 0:
                ui.warn('Aborting %s due to email without domain.\n' % operation)
                return True;

    return False;

def check_desc_for_bugnum_and_reviews(ui, changeset, operation):
    # Check first line of log of tip changeset; if it appears questionable,
    # prompt the user to confirm that they want to continue the operation.
    desc = changeset.description()
    lines = desc.split('\n')
    first_line = lines[0]
    bug_num = bugzilla_reference(first_line)
    has_review = has_reviewer_notes(first_line)

    if not bug_num or not has_review:
        ui.warn('\nQuestionable log for changeset %s:\n  %s\n'
                % (changeset,first_line))

    if not bug_num:
        ui.warn('Missing bug number, e.g. "Bug NNNNNN: ..."\n')
        response = prompt_yesno(ui, operation)
        if response == 0:
            ui.warn('Aborting %s due to missing bug number.\n' % operation)
            return True
    else:
        bug_type = check_bug_type(bug_num)
        if bug_type == BugType.SECURITY:
            ui.warn('Bug %s is a security bug.' % bug_num)
            response = prompt_yesno(ui, operation)
            if response == 0:
                ui.warn('Aborting %s due to security bug.\n' % operation)
                return True
        elif bug_type == BugType.INVALID:
            ui.warn('Bug %s is a not defined in bugzilla.' % bug_num)
            response = prompt_yesno(ui, operation)
            if response == 0:
                ui.warn('Aborting %s due to invalid bug number.\n' % operation)
                return True

    if not has_review:
        ui.warn('Missing review notes, e.g. "... (r=<name>,sr=<name>)"\n')
        response = prompt_yesno(ui, operation)
        if response == 0:
            ui.warn('Aborting %s due to missing review notes.\n' % operation)
            return True

    return False;


def diff_check(ui, repo, **kwargs):
    ui.debug('running diff_check\n')

    # get all the change contexts for this commit
    # kwargs['node'] returns the first changecontext nodeid
    changecontexts = [repo[i] for i in range(repo[kwargs['node']].rev(), len(repo))]
    # check for tabs
    def tabCheck(line):
        tab = line.find('\t')
        if tab >= 0:    # find returns -1 if not found
            return True, tab
        return False, tab

    def windowsLineendingsCheck(line):
        if line.endswith('\r'):
            return True, len(line)-1
        return False, 0

    def trailingWhitespaceCheck(line):
        if len(line.strip()) > 1:   # skip empty lines (will have a +) see bug 600536
            m = re.match(r'\+.*?(\s+$)', line)
            if m:
                return True, m.start(1)
        return False, 0
    
    def securityCheck(line):
        loc = line.find('MARK_SECURITY_CHANGE')
        if loc != -1:
            # found security change ifdef
            return True, loc
        loc = line.find('SECURITYFIX_')
        if loc != -1:
            # found security change ifdef
            return True, loc
        return False, 0

    # check for tabs - exit if user chooses to abort
    if checkChangeCtxDiff(ui, repo, changecontexts, tabCheck,
                          'Tab', ('.cpp', '.c', '.h', '.as', '.abs', '.py')):
        return True

    if checkChangeCtxDiff(ui, repo, changecontexts, windowsLineendingsCheck,
                          'Windows line ending', ('.cpp', '.c', '.h', '.as', '.abs', '.py')):
        return True

    if checkChangeCtxDiff(ui, repo, changecontexts, trailingWhitespaceCheck,
                          'Trailing Whitespace', ('.cpp', '.c', '.h', '.as', '.abs', '.py')):
        return True

    if checkChangeCtxDiff(ui, repo, changecontexts, securityCheck,
                          'Security Check', ('.cpp', '.c', '.h', '.as', '.abs', '.py')):
        return True

    return False

def checkChangeCtxDiff(ui, repo, changecontexts, testFunc, testDesc, fileEndings):
    '''Loop through each diff for each change and run the testFunc against each line'''
    ui.debug('Checking %s\n' % testDesc)
    for ctx in changecontexts:
        # Get the diff for each change and file
        for file in [f for f in ctx.files() if f.endswith(fileEndings)]:
            ui.debug('checking change: %s, file: %s\n' % (short(ctx.node()), file))
            fmatch = matchfiles(repo,[file])
            # diff from this nodes parent to current node
            diff = ''.join(patch.diff(repo, ctx.parents()[0].node(), ctx.node(), fmatch)).split('\n')
            for i in range(3, len(diff)):    # start checking after diff header
                line = diff[i]
                if line.startswith('@@'):
                    diffLocation = line
                # only check new lines added/modified in the file
                if line.startswith('+'):
                    ui.debug('\nchecking line for %s: %s\n\n' % (testDesc, line))
                    testResult, errorLocation = testFunc(line)
                    if testResult:
                        ui.warn('\n%s(s) found in %s for rev %s (change %s):\n' %
                                (testDesc, file, ctx.rev(), short(ctx.node())))
                        ui.warn('%s\n' % diffLocation)
                        ui.warn('%s\n' % line)
                        ui.warn('%s^\n' % (' '*errorLocation,)) # show a pointer to error
                        try:
                            response = ui.promptchoice('(n)o, (y)es, (a)llow %ss for current file\n' % testDesc +
                                                    'Are you sure you want to commit this change? [n]: ' ,
                                                   (('&No'), ('&Yes'), ('&Allow')), 0)
                        except AttributeError:
                            ui.warn('This commit hook requires that you have mercurial 1.4+ installed.  Please upgrade your hg installation.')
                            response = 0
                        if response == 1:
                            # next occurance in file
                            continue
                        elif response == 2:
                            # next file
                            break
                        else:
                            ui.warn('Aborting commit due to %s.\n' % testDesc)
                            # error = True
                            return True
    return False
