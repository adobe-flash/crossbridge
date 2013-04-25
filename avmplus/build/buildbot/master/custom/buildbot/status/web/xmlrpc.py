# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


from twisted.python import log
from twisted.web import xmlrpc
from buildbot.status.builder import Results
from itertools import count

class XMLRPCServer(xmlrpc.XMLRPC):
    def __init__(self):
        xmlrpc.XMLRPC.__init__(self)

    def render(self, req):
        # extract the IStatus and IControl objects for later use, since they
        # come from the request object. They'll be the same each time, but
        # they aren't available until the first request arrives.
        self.status = req.site.buildbot_service.getStatus()
        self.control = req.site.buildbot_service.getControl()
        return xmlrpc.XMLRPC.render(self, req)

    def xmlrpc_getAllBuilders(self):
        """Return a list of all builder names
        """
        log.msg("getAllBuilders")
        return self.status.getBuilderNames()

    def xmlrpc_getLastBuildResults(self, builder_name):
        """Return the result of the last build for the given builder
        """
        builder = self.status.getBuilder(builder_name)
        lastbuild = builder.getBuild(-1)
        return Results[lastbuild.getResults()]

    def xmlrpc_isBuildSetActive(self, scheduler_names):
        """Return whether the build is currently active. Treat builders that
        are offline as not building, which means they will be skipped.
        """
        
        #log.msg("isBuildSetActive() called for : %s" % (scheduler_names))
        if scheduler_names == '':
            # Get a list of builders
            buildNames = self.status.getBuilderNames()
            for builder in buildNames:
                state = self.status.getBuilder(builder).getState()[0]
                #log.msg("isBuildSetActive(): [%s] state: %s" % (builder, state))
                # If any builder is in 'building' state then we are active so return
                # Possible states are:
                #   building -> instantly return True, don't need to check any other builders
                #   idle -> we might return False if nobody is 'building'
                #   offline -> treat as idle, don't hold up any additional build requests
                #               This means that the build MAY be skipped if another request
                #               if another request comes in before it comes online
                if state == "building":
                    #log.msg("isBuildSetActive(): [%s] is building, return True" % builder)
                    return True
            
            # We found NO builders in a 'building' state. The means:
            #       a) all builders are in an idle state
            #    OR b) one or more builders are 'oflline' and other are in 'idle' state
            return False
        else:
            # Get a list of scheudulers
            schedulers = self.status.getSchedulers()
            for scheduler in schedulers:
                if scheduler.name in scheduler_names:
                    # Get a list of builders
                    buildNames = scheduler.listBuilderNames()
                    for builder in buildNames:
                        state = self.status.getBuilder(builder).getState()[0]
                        #log.msg("isBuildSetActive(): [%s-%s] state: %s" % (scheduler.name, builder, state))
                        # If any builder is in 'building' state then we are active so return
                        # Possible states are:
                        #   building -> instantly return True, don't need to check any other builders
                        #   idle -> we might return False if nobody is 'building'
                        #   offline -> treat as idle, don't hold up any additional build requests
                        #               This means that the build MAY be skipped if another request
                        #               if another request comes in before it comes online
                        if state == "building":
                            #log.msg("isBuildSetActive(): [%s-%s] is building, return True" % (scheduler.name, builder))
                            return True
            
            # We found NO builders in a 'building' state. The means:
            #       a) all builders are in an idle state
            #    OR b) one or more builders are 'oflline' and other are in 'idle' state
            #log.msg("isBuildSetActive(): False")
            return False

    def xmlrpc_stopSchedulers(self, scheduler_names):
        """Stops any active builders in the specified schedulers
        """
        if scheduler_names == '':
            # Get a list of builders
            buildNames = self.status.getBuilderNames()
            for builder in buildNames:
                state = self.status.getBuilder(builder).getState()[0]
                #log.msg("isBuildSetActive(): [%s] state: %s" % (builder, state))
                # If any builder is in 'building' state then we are active so return
                # Possible states are:
                #   building -> instantly return True, don't need to check any other builders
                #   idle -> we might return False if nobody is 'building'
                #   offline -> treat as idle, don't hold up any additional build requests
                #               This means that the build MAY be skipped if another request
                #               if another request comes in before it comes online
                if state == "building":
                    # get the internal BUILD number:
                    currBuildNum = self.status.getBuilder(builder).getCurrentBuilds()[0].getNumber()
                    
                    # Get all of the processBuilder objects, these actaully control the build and
                    # are not just objects that collect status, these are what we can actually stop
                    allProcBuilders = self.status.botmaster.getBuilders()
                    
                    # loop through ALL of the builders and find the one that matches the status builder 
                    # that we have found to be actively building
                    for procBuilder in allProcBuilders:
                        if procBuilder.name == builder:
                            # get the actual process.Build and stop it
                            procBuilder.getBuild(currBuildNum).stopBuild("Stopping build by user request")
            
        else:
            # Get a list of scheudulers
            schedulers = self.status.getSchedulers()
            for scheduler in schedulers:
                if scheduler.name in scheduler_names:
                    # Get a list of builders
                    buildNames = scheduler.listBuilderNames()
                    for builder in buildNames:
                        state = self.status.getBuilder(builder).getState()[0]
                        # If any builder is in 'building' state then we are going to stop it
                        # Possible states are:
                        #   building -> instantly return True, don't need to check any other builders
                        #   idle -> we might return False if nobody is 'building'
                        #   offline -> treat as idle, don't hold up any additional build requests
                        #               This means that the build MAY be skipped if another request
                        #               if another request comes in before it comes online
                        if state == "building":
                            # get the internal BUILD number:
                            currBuildNum = self.status.getBuilder(builder).getCurrentBuilds()[0].getNumber()
                            
                            # Get all of the processBuilder objects, these actaully control the build and
                            # are not just objects that collect status, these are what we can actually stop
                            allProcBuilders = self.status.botmaster.getBuilders()
                            
                            # loop through ALL of the builders and find the one that matches the status builder 
                            # that we have found to be actively building
                            for procBuilder in allProcBuilders:
                                if procBuilder.name == builder:
                                    # get the actual process.Build and stop it
                                    procBuilder.getBuild(currBuildNum).stopBuild("Stopping build by user request")



    def xmlrpc_getLastBuilds(self, builder_name, num_builds):
        """Return the last N completed builds for the given builder.
        'builder_name' is the name of the builder to query
        'num_builds' is the number of builds to return

	Each build is returned in the same form as xmlrpc_getAllBuildsInInterval
        """
        log.msg("getLastBuilds: %s - %d" % (builder_name, num_builds))
        builder = self.status.getBuilder(builder_name)
        all_builds = []
        for build_number in range(1, num_builds+1):
            build = builder.getBuild(-build_number)
            if not build:
                break
            if not build.isFinished():
                continue
            (build_start, build_end) = build.getTimes()

            ss = build.getSourceStamp()
            branch = ss.branch
            if branch is None:
                branch = ""
            try:
                revision = build.getSourceStamp().revision
            except KeyError:
                revision = ""
            revision = str(revision)

            answer = (builder_name,
                      build.getNumber(),
                      build_end,
                      branch,
                      revision,
                      Results[build.getResults()],
                      build.getText(),
                      )
            all_builds.append((build_end, answer))

        # now we've gotten all the builds we're interested in. Sort them by
        # end time.
        all_builds.sort(lambda a,b: cmp(a[0], b[0]))
        # and remove the timestamps
        all_builds = [t[1] for t in all_builds]

        log.msg("ready to go: %s" % (all_builds,))

        return all_builds


    def xmlrpc_getAllBuildsInInterval(self, start, stop):
        """Return a list of builds that have completed after the 'start'
        timestamp and before the 'stop' timestamp. This looks at all
        Builders.

        The timestamps are integers, interpreted as standard unix timestamps
        (seconds since epoch).

        Each Build is returned as a tuple in the form::
         (buildername, buildnumber, build_end, branchname, revision,
          results, text)

        The buildnumber is an integer. 'build_end' is an integer (seconds
        since epoch) specifying when the build finished.

        The branchname is a string, which may be an empty string to indicate
        None (i.e. the default branch). The revision is a string whose
        meaning is specific to the VC system in use, and comes from the
        'got_revision' build property. The results are expressed as a string,
        one of ('success', 'warnings', 'failure', 'exception'). The text is a
        list of short strings that ought to be joined by spaces and include
        slightly more data about the results of the build.
        """
        #log.msg("start: %s %s %s" % (start, type(start), start.__class__))
        log.msg("getAllBuildsInInterval: %d - %d" % (start, stop))
        all_builds = []

        for builder_name in self.status.getBuilderNames():
            builder = self.status.getBuilder(builder_name)
            for build_number in count(1):
                build = builder.getBuild(-build_number)
                if not build:
                    break
                if not build.isFinished():
                    continue
                (build_start, build_end) = build.getTimes()
                # in reality, builds are mostly ordered by start time. For
                # the purposes of this method, we pretend that they are
                # strictly ordered by end time, so that we can stop searching
                # when we start seeing builds that are outside the window.
                if build_end > stop:
                    continue # keep looking
                if build_end < start:
                    break # stop looking

                ss = build.getSourceStamp()
                branch = ss.branch
                if branch is None:
                    branch = ""
                try:
                    revision = build.getProperty("got_revision")
                except KeyError:
                    revision = ""
                revision = str(revision)

                answer = (builder_name,
                          build.getNumber(),
                          build_end,
                          branch,
                          revision,
                          Results[build.getResults()],
                          build.getText(),
                          )
                all_builds.append((build_end, answer))
            # we've gotten all the builds that we care about from this
            # particular builder, so now we can continue on the next builder

        # now we've gotten all the builds we're interested in. Sort them by
        # end time.
        all_builds.sort(lambda a,b: cmp(a[0], b[0]))
        # and remove the timestamps
        all_builds = [t[1] for t in all_builds]

        log.msg("ready to go: %s" % (all_builds,))

        return all_builds

    def xmlrpc_getBuild(self, builder_name, build_number):
        """Return information about a specific build.

        """
        builder = self.status.getBuilder(builder_name)
        build = builder.getBuild(build_number)
        info = {}
        info['builder_name'] = builder.getName()
        info['url'] = self.status.getURLForThing(build) or ''
        info['reason'] = build.getReason()
        info['slavename'] = build.getSlavename()
        info['results'] = build.getResults()
        info['text'] = build.getText()
        # Added to help out requests for build -N
        info['number'] = build.number
        ss = build.getSourceStamp()
        branch = ss.branch
        if branch is None:
            branch = ""
        info['branch'] = str(branch)
        try:
            revision = str(build.getProperty("got_revision"))
        except KeyError:
            revision = ""
        info['revision'] = str(revision)
        info['start'], info['end'] = build.getTimes()

        info_steps = []
        for s in build.getSteps():
            stepinfo = {}
            stepinfo['name'] = s.getName()
            stepinfo['start'], stepinfo['end'] = s.getTimes()
            stepinfo['results'] = s.getResults()
            info_steps.append(stepinfo)
        info['steps'] = info_steps

        info_logs = []
        for l in build.getLogs():
            loginfo = {}
            loginfo['name'] = l.getStep().getName() + "/" + l.getName()
            #loginfo['text'] = l.getText()
            loginfo['text'] = "HUGE"
            info_logs.append(loginfo)
        info['logs'] = info_logs
        return info

