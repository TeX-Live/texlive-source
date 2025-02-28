--GregorioTeX Signs Lua support file.
--
--Copyright (C) 2015-2025 The Gregorio Project (see CONTRIBUTORS.md)
--
--This file is part of Gregorio.
--
--Gregorio is free software: you can redistribute it and/or modify
--it under the terms of the GNU General Public License as published by
--the Free Software Foundation, either version 3 of the License, or
--(at your option) any later version.
--
--Gregorio is distributed in the hope that it will be useful,
--but WITHOUT ANY WARRANTY; without even the implied warranty of
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--GNU General Public License for more details.
--
--You should have received a copy of the GNU General Public License
--along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

-- this file contains lua functions to support signs used by GregorioTeX.

-- GREGORIO_VERSION 6.1.0

local err = gregoriotex.module.err
local warn = gregoriotex.module.warn
local info = gregoriotex.module.info
local log = gregoriotex.module.log
local debugmessage = gregoriotex.module.debugmessage

-- Note offset cases:
-- here are the common values for both hepisema (and consequently also for
-- additional lines) and vepisema
-- this indicates the note
local offset_cases = {
  -- punctum as last note (works with pes)
  {
    case = 'FinalPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctum}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- deminutus as last note
  {
    case = 'FinalDeminutus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumDeminutus}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumDeminutus}{\gre@char@he@initio{#4}}{2}{#3}]],
  },
  -- second-to-last note, disconnected from prior note, with a second ambitus
  -- of at least two, when last note is a standard punctum (like the second
  -- note of hig)
  {
    case = 'PenultBeforePunctumWide',
    v = [[\gre@vepisemaorrareaux{\GreCPFlexusNobarTwoNothing}{\GreCPPunctum}{2}{0}{#2}{#3}{#4}]],
    -- a kind of flexus, it has the good width
    h = [[\gre@hepisorlineaux{\GreCPFlexusNobarTwoNothing}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- second-to-last note, when last note is a deminutus
  {
    case = 'PenultBeforeDeminutus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPFlexusTwoDeminutus}{1}{0}{#2}{#3}{#4}]],
    -- in order to go to the good place, we first make a kern of - the glyph
    -- before deminutus, which has the same width as a standard flexus deminutus
    h = [[\gre@hepisorlineaux{\GreCPFlexusTwoDeminutus}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- third-to-last note, when the last note is a punctum (for porrectus flexus)
  {
    case = 'AntepenultBeforePunctum',
    v = [[\gre@vepisemaorrareaux{\GreCPTorculusOneTwoNothing}{\GreCPPunctum}{2}{0}{#2}{#3}{#4}]],
    -- is a torculus, it has the good width
    h = [[\gre@hepisorlineaux{\GreCPTorculusOneTwoNothing}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- third-to-last note, when the last notes is a deminutus (for porrectus
  -- flexus)
  {
    case = 'AntepenultBeforeDeminutus',
    v = [[\gre@vepisemaorrareaux{\GreCPTorculusTwoTwoDeminutus}{\GreCPPunctum}{2}{0}{#2}{#3}{#4}]],
    -- torculus deminutus has the good width
    h = [[\gre@hepisorlineaux{\GreCPTorculusTwoTwoDeminutus}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- standard punctum as first note, disconnected from next note
  {
    case = 'InitialPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@punctum{#4}}{0}{#3}]],
  },
  -- initio debilis (always the first note)
  {
    case = 'InitioDebilis',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumDeminutus}{0}{0}{#2}{#3}{#4}]],
    -- we assume that the initio-debilis has the same width as a punctum
    -- deminutus
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@initio{#4}}{0}{#3}]],
  },
  -- first note of a non-auctus porrectus with a second ambitus of at least two
  {
    case = 'PorrNonAuctusInitialWide',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{0}{0}{#2}{#3}{#4}]],
    -- we do (for now) the same as case 6
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@porrectus{#2}{#4}}{0}{#3}]],
  },
  -- first note of a non-auctus porrectus with a second ambitus of one
  {
    case = 'PorrNonAuctusInitialOne',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@porrectus@amone{#2}{#4}}{0}{#3}]],
  },
  -- first note of an auctus porrectus, regardless of second ambitus
  {
    case = 'PorrAuctusInitialAny',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@porrectusfl{#2}{#4}}{0}{#3}]],
  },
  -- punctum inclinatum as last note
  {
    case = 'FinalInclinatum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPDescendensPunctumInclinatum}{0}{30\the\gre@factor }{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPDescendensPunctumInclinatum}{\gre@char@he@inclinatum{#4}}{2}{#3}]],
  },
  -- punctum inclinatum deminutus as last note
  {
    case = 'FinalInclinatumDeminutus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumInclinatumDeminutus}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumInclinatumDeminutus}{\gre@char@he@inclinatumdem{#4}}{2}{#3}]],
  },
  -- stropha as last note
  {
    case = 'FinalStropha',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPStropha}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPStropha}{\gre@char@he@stropha{#4}}{2}{#3}]],
  },
  -- quilisma as last note
  {
    case = 'FinalQuilisma',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPQuilisma}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPQuilisma}{\gre@char@he@quilisma{#4}}{2}{#3}]],
  },
  -- oriscus as last note
  {
    case = 'FinalOriscus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPAscendensOriscus}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPAscendensOriscus}{\gre@char@he@oriscus{#4}}{2}{#3}]],
  },
  -- second-to-last note, with a second ambitus of one, when last note is a
  -- standard punctum (like the second note of ghg)
  {
    case = 'PenultBeforePunctumOne',
    v = [[\gre@vepisemaorrareaux{\GreCPFlexusNobarOneNothing}{\GreCPPunctum}{2}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlexusNobarOneNothing}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- "upper smaller punctum" as last note (concerning simple podatus, podatus,
  -- and torculus resupinus)
  {
    case = 'FinalUpperPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumSmall}{1}{-30\the\gre@factor}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumSmall}{\gre@char@he@smallpunctum{#4}}{2}{#3}]],
  },
  -- oriscus as first note, disconnected from next note
  {
    case = 'InitialOriscus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPDescendensOriscus}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@oriscus{#4}}{0}{#3}]],
  },
  -- quilisma as first note, disconnected from next note
  {
    case = 'InitialQuilisma',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPQuilisma}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@quilisma{#4}}{0}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a punctum,
  -- with a first and second ambitus of at least two
  {
    case = 'TorcResNonAuctusSecondWideWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumTwo}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a punctum,
  -- with a first ambitus of one and a second ambitus of at least two
  {
    case = 'TorcResNonAuctusSecondOneWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOne}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus initio debilis with any
  -- first ambitus and a second ambitus of at least two
  {
    case = 'TorcResDebilisNonAuctusSecondAnyWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@debilis}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOneInitioDebilis}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- linea punctum (cavum) as last note
  {
    case = 'FinalLineaPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPLineaPunctum}{1}{0}{#2}{#3}{#4}]],
    -- the episema is not quite long enough so I assumed a different width
    -- for now...
    h = [[\gre@hepisorlineaux{\GreCPPesQuadratumOneInitioDefilisDescendens}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- standard bar
  {
    case = 'BarStandard',
    v = [[\gre@vepisemaorrareaux{0}{\gre@char@bar@divisiominima}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\gre@char@bar@divisiominima}{\gre@char@he@barstandard{#4}}{2}{#3}]],
  },
  -- virgula
  {
    case = 'BarVirgula',
    v = [[\gre@vepisemaorrareaux{0}{\gre@char@bar@virgula}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\gre@char@bar@virgula}{\gre@char@he@barvirgula{#4}}{2}{#3}]],
  },
  -- divisio finalis
  {
    case = 'BarDivisioFinalis',
    v = [[\gre@vepisemaorrareaux{0}{\gre@fontchar@divisiofinalis}{1}{0}{#2}{#3}{#4}]],
  },
  -- parenthesized bar
  {
    case = 'BarParen',
    v = [[\gre@vepisemaorrareaux{0}{\gre@char@bar@divisiominimaparen}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\gre@char@bar@divisiominimaparen}{\gre@char@he@barparen{#4}}{2}{#3}]],
  },
  -- parenthesized virgula
  {
    case = 'BarVirgulaParen',
    v = [[\gre@vepisemaorrareaux{0}{\gre@char@bar@virgulaparen}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\gre@char@bar@virgulaparen}{\gre@char@he@barvirgulaparen{#4}}{2}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a quilisma,
  -- with a first and second ambitus of at least two
  {
    case = 'TorcResQuilismaNonAuctusSecondWideWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaTwo}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with an oriscus,
  -- with a first and second ambitus of at least two
  {
    case = 'TorcResOriscusNonAuctusSecondWideWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusTwo}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a quilisma,
  -- with a first ambitus of one and and second ambitus of at least two
  {
    case = 'TorcResQuilismaNonAuctusSecondOneWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaOne}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with an oriscus,
  -- with a first ambitus of one and and second ambitus of at least two
  {
    case = 'TorcResOriscusNonAuctusSecondOneWide',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusOne}{\gre@char@he@porrectus{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a punctum,
  -- with a first ambitus of at least two and a second ambitus of one
  {
    case = 'TorcResNonAuctusSecondWideOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumTwo}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus initio debilis with any
  -- first ambitus and a second ambitus of one
  {
    case = 'TorcResDebilisNonAuctusSecondAnyOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@debilis}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOneInitioDebilis}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a quilisma,
  -- with a first ambitus of at least two and a second ambitus of one
  {
    case = 'TorcResQuilismaNonAuctusSecondWideOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaTwo}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with an oriscus,
  -- with a first ambitus of at least two and a second ambitus of one
  {
    case = 'TorcResOriscusNonAuctusSecondWideOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusTwo}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a punctum,
  -- with a first and second ambitus of one
  {
    case = 'TorcResNonAuctusSecondOneOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOne}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with a quilisma,
  -- with a first and second ambitus of one
  {
    case = 'TorcResQuilismaNonAuctusSecondOneOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaOne}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of a non-auctus torculus resupinus starting with an oriscus,
  -- with a first and second ambitus of one
  {
    case = 'TorcResOriscusNonAuctusSecondOneOne',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusOne}{\gre@char@he@porrectus@amone{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with a punctum, with
  -- a first ambitus of at least two and any second ambitus
  {
    case = 'TorcResAuctusSecondWideAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumTwo}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus initio debilis with any first
  -- and second ambitus
  {
    case = 'TorcResDebilisAuctusSecondAnyAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@debilis}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOneInitioDebilis}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with a quilisma, with
  -- a first ambitus of at least two and any second ambitus
  {
    case = 'TorcResQuilismaAuctusSecondWideAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaTwo}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with an oriscus, with
  -- a first ambitus of at least two and any second ambitus
  {
    case = 'TorcResOriscusAuctusSecondWideAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@two}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusTwo}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with a punctum, with
  -- a first ambitus of one and any second ambitus
  {
    case = 'TorcResAuctusSecondOneAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@punctum@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingPunctumOne}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with a quilisma, with
  -- a first ambitus of one and any second ambitus
  {
    case = 'TorcResQuilismaAuctusSecondOneAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@quilisma@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingQuilismaOne}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second note of an auctus torculus resupinus starting with an oriscus, with
  -- a first ambitus of one and any second ambitus
  {
    case = 'TorcResOriscusAuctusSecondOneAny',
    v = [[\gre@vepisemaorrareaux{\gre@char@fuse@oriscus@one}{\GreCPPunctum}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPLeadingOriscusOne}{\gre@char@he@porrectusfl{#2}{#4}}{3}{#3}]],
  },
  -- second-to-last note connected to prior note, with a second ambitus of at
  -- least two, when last note is a standard punctum (like the second note of
  -- gig)
  {
    case = 'ConnectedPenultBeforePunctumWide',
    v = [[\gre@vepisemaorrareaux{\GreCPFlexusLineBL}{\GreCPPunctumLineBLBR}{2}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlexusLineBL}{\gre@char@he@punctum@line@blbr{#4}}{2}{#3}]],
  },
  -- second-to-last note connected to prior note, with a second ambitus of one,
  -- when last note is a standard punctum (like the second note of gih)
  {
    case = 'ConnectedPenultBeforePunctumOne',
    v = [[\gre@vepisemaorrareaux{\GreCPFlexusAmOneLineBL}{\GreCPPunctumLineBL}{2}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlexusAmOneLineBL}{\gre@char@he@punctum@line@bl{#4}}{2}{#3}]],
  },
  -- standard punctum as first note, connected to next higher note
  {
    case = 'InitialConnectedPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumLineTR}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@punctum@line@tr{#4}}{0}{#3}]],
  },
  -- "virga" as first note, connected to next lower note
  {
    case = 'InitialConnectedVirga',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPVirgaBaseLineBL}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@virgabase@line@bl{#4}}{0}{#3}]],
  },
  -- quilisma as first note, connected to next higher note
  {
    case = 'InitialConnectedQuilisma',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPQuilismaLineTR}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@quilisma@line@tr{#4}}{0}{#3}]],
  },
  -- oriscus as first note, connected to next higher note
  {
    case = 'InitialConnectedOriscus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPAscendensOriscusLineTR}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@oriscus@line@tr{#4}}{0}{#3}]],
  },
  -- punctum as last note, connected to prior higher note
  {
    case = 'FinalConnectedPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumLineTL}{\gre@char@he@punctum@line@tl{#4}}{2}{#3}]],
  },
  -- auctus as last note, connected to prior lower note
  {
    case = 'FinalConnectedAuctus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctumAuctusLineBL}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumAuctusLineBL}{\gre@char@he@punctumauctus@line@bl{#4}}{2}{#3}]],
  },
  -- virga aucta as last note
  {
    case = 'FinalVirgaAuctus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPVirgaReversaDescendens}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPVirgaReversaDescendens}{\gre@char@he@punctumauctus@line@bl{#4}}{2}{#3}]],
  },
  -- "virga" as last note, connected to prior lower note
  {
    case = 'FinalConnectedVirga',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPVirga}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPVirga}{\gre@char@he@virga{#4}}{2}{#3}]],
  },
  -- "virga" as first note, disconnected from next note
  {
    case = 'InitialVirga',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPVirga}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{0}{\gre@char@he@virga{#4}}{0}{#3}]],
  },
  -- "oriscus" as the middle note of a salicus with a second ambitus of at
  -- least two
  {
    case = 'SalicusOriscusWide',
    v = [[\gre@vepisemaorrareaux{\GreCPPesAscendensOriscusThreeNothing}{\GreCPAscendensOriscusLineBLTR}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPesAscendensOriscusThreeNothing}{\gre@char@he@salicus@oriscus{#4}}{4}{#3}]],
  },
  -- "oriscus" as the middle note of a salicus with a second ambitus of one
  {
    case = 'SalicusOriscusOne',
    v = [[\gre@vepisemaorrareaux{\GreCPPesAscendensOriscusOneNothing}{\GreCPAscendensOriscus}{3}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPesAscendensOriscusOneNothing}{\gre@char@he@salicus@oriscus{#4}}{4}{#3}]],
  },
  -- punctum fused to the next note
  {
    case = 'LeadingPunctum',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPPunctum}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPPunctumTwoUp}{\gre@char@he@punctum{#4}}{2}{#3}]],
  },
  -- qulisma fused to the next note
  {
    case = 'LeadingQuilisma',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPQuilisma}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPQuilismaTwoUp}{\gre@char@he@quilisma{#4}}{2}{#3}]],
  },
  -- oriscus fused to the next note
  {
    case = 'LeadingOriscus',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPAscendensOriscus}{0}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPAscendensOriscusTwoUp}{\gre@char@he@oriscus{#4}}{2}{#3}]],
  },
  -- flat
  {
    case = 'Flat',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPFlat}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlat}{\gre@char@he@flat{#4}}{2}{#3}]],
  },
  -- sharp
  {
    case = 'Sharp',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPSharp}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPSharp}{\gre@char@he@sharp{#4}}{2}{#3}]],
  },
  -- natural
  {
    case = 'Natural',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPNatural}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPNatural}{\gre@char@he@natural{#4}}{2}{#3}]],
  },
  -- parenthesized flat
  {
    case = 'FlatParen',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPFlatParen}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlatParen}{\gre@char@he@flatparen{#4}}{2}{#3}]],
  },
  -- parenthesized sharp
  {
    case = 'SharpParen',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPSharpParen}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPSharpParen}{\gre@char@he@sharpparen{#4}}{2}{#3}]],
  },
  -- parenthesized natural
  {
    case = 'NaturalParen',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPNaturalParen}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPNaturalParen}{\gre@char@he@naturalparen{#4}}{2}{#3}]],
  },
  -- soft flat
  {
    case = 'FlatSoft',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPFlat}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPFlat}{\gre@char@he@flat{#4}}{2}{#3}]],
  },
  -- soft sharp
  {
    case = 'SharpSoft',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPSharp}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPSharp}{\gre@char@he@sharp{#4}}{2}{#3}]],
  },
  -- soft natural
  {
    case = 'NaturalSoft',
    v = [[\gre@vepisemaorrareaux{0}{\GreCPNatural}{1}{0}{#2}{#3}{#4}]],
    h = [[\gre@hepisorlineaux{\GreCPNatural}{\gre@char@he@natural{#4}}{2}{#3}]],
  },
}

local function emit_offset_macros()
  local i, item
  for i, item in ipairs(offset_cases) do
    debugmessage('offsetcase', [[\def\GreOCase%s{%d}]], item.case, i)
    tex.sprint(string.format([[\def\GreOCase%s{%d}]], item.case, i))
  end
  tex.sprint([[\def\gre@v@case#1#2#3#4{]])
  tex.sprint([[\ifcase#1\gre@bug{Invalid note offset case: \string#1}]])
  for i, item in ipairs(offset_cases) do
    tex.sprint([[\or]])
    if item.v then
      tex.sprint(item.v)
    end
  end
  tex.sprint([[\else\gre@bug{Invalid note 2 offset case: \string#1}]])
  tex.sprint([[\fi}]])
  tex.sprint([[\def\gre@h@case#1#2#3#4{]])
  tex.sprint([[\ifcase#1\gre@bug{Invalid note offset case: \string#1}]])
  for i, item in ipairs(offset_cases) do
    tex.sprint([[\or]])
    if item.h then
      tex.sprint(item.h)
    end
  end
  tex.sprint([[\else\gre@bug{Invalid note offset case: \string#1}]])
  tex.sprint([[\fi}]])
end

gregoriotex.emit_offset_macros   = emit_offset_macros
