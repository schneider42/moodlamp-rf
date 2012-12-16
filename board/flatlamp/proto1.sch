<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.1">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="9" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="9" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="led">
<packages>
<package name="P-LCC-6_TOPLED">
<description>&lt;b&gt;P-LCC-6 TOPLED MULTILED®&lt;/b&gt;&lt;p&gt;
Source: OSRAM - LRTB_G6TG.pdf</description>
<wire x1="-1.7" y1="1.55" x2="1.7" y2="1.55" width="0.1016" layer="51"/>
<wire x1="1.7" y1="1.55" x2="1.7" y2="-1.55" width="0.1016" layer="21"/>
<wire x1="1.7" y1="-1.55" x2="-1.25" y2="-1.55" width="0.1016" layer="51"/>
<wire x1="-1.25" y1="-1.55" x2="-1.7" y2="-1.55" width="0.1016" layer="51"/>
<wire x1="-1.7" y1="-1.55" x2="-1.7" y2="-1.1" width="0.1016" layer="21"/>
<wire x1="-1.7" y1="-1.1" x2="-1.7" y2="1.55" width="0.1016" layer="21"/>
<wire x1="-1.7" y1="-1.1" x2="-1.25" y2="-1.55" width="0.1016" layer="51"/>
<wire x1="-0.8" y1="-1.2" x2="-1.2" y2="-0.8" width="0.1016" layer="51" curve="-90"/>
<wire x1="-1.2" y1="-0.8" x2="-1.2" y2="0.8" width="0.1016" layer="51"/>
<wire x1="-1.2" y1="0.8" x2="-0.8" y2="1.2" width="0.1016" layer="51" curve="-90"/>
<wire x1="-0.8" y1="1.2" x2="0.8" y2="1.2" width="0.1016" layer="51"/>
<wire x1="0.8" y1="1.2" x2="1.2" y2="0.8" width="0.1016" layer="51" curve="-90"/>
<wire x1="1.2" y1="0.8" x2="1.2" y2="-0.8" width="0.1016" layer="51"/>
<wire x1="1.2" y1="-0.8" x2="0.8" y2="-1.2" width="0.1016" layer="51" curve="-90"/>
<wire x1="0.8" y1="-1.2" x2="-0.8" y2="-1.2" width="0.1016" layer="51"/>
<wire x1="0.475" y1="-0.825" x2="0.775" y2="-0.525" width="0.1016" layer="51" curve="90"/>
<wire x1="0.775" y1="-0.525" x2="0.775" y2="0.525" width="0.1016" layer="21"/>
<wire x1="0.775" y1="0.525" x2="0.475" y2="0.825" width="0.1016" layer="51" curve="90"/>
<wire x1="0.475" y1="0.825" x2="-0.475" y2="0.825" width="0.1016" layer="51"/>
<wire x1="-0.475" y1="0.825" x2="-0.775" y2="0.525" width="0.1016" layer="51" curve="90"/>
<wire x1="-0.775" y1="0.525" x2="-0.775" y2="-0.525" width="0.1016" layer="21"/>
<wire x1="-0.775" y1="-0.525" x2="-0.475" y2="-0.825" width="0.1016" layer="51" curve="90"/>
<wire x1="-0.475" y1="-0.825" x2="0.475" y2="-0.825" width="0.1016" layer="51"/>
<wire x1="-1.7" y1="-1.1" x2="-1.45" y2="-1.35" width="0.1016" layer="21"/>
<wire x1="-1.475" y1="-1.55" x2="-1.7" y2="-1.55" width="0.1016" layer="21"/>
<circle x="-4" y="-4" radius="0.2015" width="0" layer="29"/>
<smd name="A3" x="2.65" y="2.7" dx="4" dy="4" layer="1" stop="no" cream="no"/>
<smd name="A2" x="2.65" y="-2.7" dx="4" dy="4" layer="1" stop="no" cream="no"/>
<smd name="A1" x="-2.65" y="-2.7" dx="4" dy="4" layer="1" stop="no" cream="no"/>
<smd name="C1" x="-2.65" y="2.7" dx="4" dy="4" layer="1" stop="no" cream="no"/>
<smd name="C3" x="0" y="2.7" dx="0.6" dy="4" layer="1" stop="no" cream="no"/>
<smd name="C2" x="0" y="-2.7" dx="0.6" dy="4" layer="1" stop="no" cream="no"/>
<text x="-4.075" y="5.34" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.25" y="-7.34" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="0.6" y1="-2.35" x2="1.45" y2="-0.65" layer="29"/>
<rectangle x1="-0.35" y1="-2.35" x2="0.35" y2="-0.65" layer="29"/>
<rectangle x1="-1.45" y1="-2.35" x2="-0.6" y2="-0.65" layer="29"/>
<rectangle x1="-1.45" y1="0.675" x2="-0.6" y2="2.375" layer="29"/>
<rectangle x1="-0.35" y1="0.65" x2="0.35" y2="2.35" layer="29"/>
<rectangle x1="0.6" y1="0.65" x2="1.45" y2="2.35" layer="29"/>
<rectangle x1="0.7" y1="-2.275" x2="1.4" y2="-0.75" layer="31"/>
<rectangle x1="-0.25" y1="-2.275" x2="0.25" y2="-0.75" layer="31"/>
<rectangle x1="-1.4" y1="-2.275" x2="-0.7" y2="-0.75" layer="31"/>
<rectangle x1="0.7" y1="0.75" x2="1.4" y2="2.275" layer="31"/>
<rectangle x1="-0.25" y1="0.75" x2="0.25" y2="2.275" layer="31"/>
<rectangle x1="-1.4" y1="0.75" x2="-0.7" y2="2.275" layer="31"/>
<rectangle x1="-1.4" y1="1.525" x2="-0.7" y2="1.8" layer="51"/>
<rectangle x1="-0.25" y1="1.55" x2="0.25" y2="1.8" layer="51"/>
<rectangle x1="0.7" y1="1.575" x2="1.4" y2="1.8" layer="51"/>
<rectangle x1="-1.4" y1="-1.8" x2="-0.7" y2="-1.575" layer="51"/>
<rectangle x1="-0.25" y1="-1.8" x2="0.25" y2="-1.575" layer="51"/>
<rectangle x1="0.7" y1="-1.8" x2="1.4" y2="-1.575" layer="51"/>
<rectangle x1="-0.375" y1="-0.125" x2="-0.125" y2="0.125" layer="21"/>
<rectangle x1="0.125" y1="0.175" x2="0.375" y2="0.425" layer="21"/>
<rectangle x1="0.125" y1="-0.425" x2="0.375" y2="-0.175" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="LED">
<wire x1="1.27" y1="0" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.27" y2="0" width="0.254" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.27" y2="-2.54" width="0.254" layer="94"/>
<wire x1="1.27" y1="0" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="-1.27" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="-2.032" y1="-0.762" x2="-3.429" y2="-2.159" width="0.1524" layer="94"/>
<wire x1="-1.905" y1="-1.905" x2="-3.302" y2="-3.302" width="0.1524" layer="94"/>
<text x="3.556" y="-4.572" size="1.778" layer="95" rot="R90">&gt;NAME</text>
<text x="5.715" y="-4.572" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="C" x="0" y="-5.08" visible="off" length="short" direction="pas" rot="R90"/>
<pin name="A" x="0" y="2.54" visible="off" length="short" direction="pas" rot="R270"/>
<polygon width="0.1524" layer="94">
<vertex x="-3.429" y="-2.159"/>
<vertex x="-3.048" y="-1.27"/>
<vertex x="-2.54" y="-1.778"/>
</polygon>
<polygon width="0.1524" layer="94">
<vertex x="-3.302" y="-3.302"/>
<vertex x="-2.921" y="-2.413"/>
<vertex x="-2.413" y="-2.921"/>
</polygon>
</symbol>
</symbols>
<devicesets>
<deviceset name="MULTILED_LRTB_G6TG" prefix="LED">
<description>&lt;b&gt;6-lead MULTILED® Enhanced optical Power LED (ThinFilm® / ThinGaN®)&lt;/b&gt;&lt;p&gt;
Source. OSRAM - LRTB_G6TG.pdf</description>
<gates>
<gate name="R" symbol="LED" x="-7.62" y="0" addlevel="always"/>
<gate name="G" symbol="LED" x="2.54" y="0" addlevel="always"/>
<gate name="B" symbol="LED" x="12.7" y="0" addlevel="always"/>
</gates>
<devices>
<device name="" package="P-LCC-6_TOPLED">
<connects>
<connect gate="B" pin="A" pad="A3"/>
<connect gate="B" pin="C" pad="C3"/>
<connect gate="G" pin="A" pad="A2"/>
<connect gate="G" pin="C" pad="C2"/>
<connect gate="R" pin="A" pad="A1"/>
<connect gate="R" pin="C" pad="C1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="con-jack">
<description>&lt;b&gt;Jacks&lt;/b&gt;&lt;p&gt;
Power Connectors&lt;br&gt;
Phone Connectors&lt;br&gt;
Cinch Connectors&lt;br&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="DCJ0303">
<description>&lt;b&gt;DC POWER JACK&lt;/b&gt; Pad shape changed to LONG 2007.07.26&lt;p&gt;
Source: DCJ0303.pdf</description>
<wire x1="1.65" y1="2.6" x2="-1.65" y2="2.6" width="0" layer="46"/>
<wire x1="-1.65" y1="2.6" x2="-1.65" y2="3.6" width="0" layer="46"/>
<wire x1="-1.65" y1="3.6" x2="1.65" y2="3.6" width="0" layer="46"/>
<wire x1="1.65" y1="3.6" x2="1.65" y2="2.6" width="0" layer="46"/>
<wire x1="5.3" y1="1.4" x2="4.3" y2="1.4" width="0" layer="46"/>
<wire x1="4.3" y1="1.4" x2="4.3" y2="-1.4" width="0" layer="46"/>
<wire x1="4.3" y1="-1.4" x2="5.3" y2="-1.4" width="0" layer="46"/>
<wire x1="5.3" y1="-1.4" x2="5.3" y2="1.4" width="0" layer="46"/>
<wire x1="1.4" y1="-3.5" x2="-1.4" y2="-3.5" width="0" layer="46"/>
<wire x1="-1.4" y1="-3.5" x2="-1.4" y2="-2.5" width="0" layer="46"/>
<wire x1="-1.4" y1="-2.5" x2="1.4" y2="-2.5" width="0" layer="46"/>
<wire x1="1.4" y1="-2.5" x2="1.4" y2="-3.5" width="0" layer="46"/>
<wire x1="-4.3" y1="-10.4" x2="4.3" y2="-10.4" width="0.2032" layer="21"/>
<wire x1="4.3" y1="-10.4" x2="4.3" y2="3.9" width="0.2032" layer="51"/>
<wire x1="4.3" y1="3.9" x2="-4.3" y2="3.9" width="0.2032" layer="51"/>
<wire x1="-4.3" y1="3.9" x2="-4.3" y2="-10.4" width="0.2032" layer="21"/>
<wire x1="-2.7" y1="3.9" x2="-4.3" y2="3.9" width="0.2032" layer="21"/>
<wire x1="4.3" y1="3.9" x2="2.7" y2="3.9" width="0.2032" layer="21"/>
<wire x1="-3" y1="-10.275" x2="-3" y2="-3" width="0.2032" layer="51" style="shortdash"/>
<wire x1="3" y1="-10.3" x2="3" y2="-3" width="0.2032" layer="51" style="shortdash"/>
<wire x1="3" y1="-3" x2="-3" y2="-3" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-0.9" y1="-9" x2="-0.9" y2="-4.5" width="0.2032" layer="51" style="shortdash"/>
<wire x1="0.9" y1="-9" x2="0.9" y2="-4.5" width="0.2032" layer="51" style="shortdash"/>
<wire x1="-0.9" y1="-9" x2="0.9" y2="-9" width="0.2032" layer="51" curve="166.57811"/>
<wire x1="4.3" y1="-10.4" x2="4.3" y2="-2.45" width="0.2032" layer="21"/>
<wire x1="4.3" y1="2.3" x2="4.3" y2="3.9" width="0.2032" layer="21"/>
<pad name="1" x="0" y="3.1" drill="1" diameter="2" shape="long" rot="R180"/>
<pad name="3" x="0" y="-3" drill="1" diameter="2" shape="long" rot="R180"/>
<pad name="2" x="4.8" y="0" drill="1" diameter="2" shape="long" rot="R90"/>
<text x="6.35" y="-10.16" size="1.27" layer="25" rot="R90">&gt;NAME</text>
<text x="-5.08" y="-10.16" size="1.27" layer="27" rot="R90">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="DC-JACK-SWITCH">
<wire x1="5.08" y1="2.54" x2="-2.54" y2="2.54" width="0.1524" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="1.27" width="0.1524" layer="94"/>
<wire x1="5.08" y1="0" x2="2.54" y2="0" width="0.1524" layer="94"/>
<wire x1="2.54" y1="0" x2="2.54" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-2.54" x2="2.54" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="0.762" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="0.762" y1="-2.54" x2="0" y2="-1.016" width="0.1524" layer="94"/>
<wire x1="0" y1="-1.016" x2="-0.762" y2="-2.54" width="0.1524" layer="94"/>
<text x="-2.54" y="3.81" size="1.778" layer="95">&gt;NAME</text>
<text x="-2.54" y="-6.35" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="-3.302" y1="-2.54" x2="-1.778" y2="1.27" layer="94"/>
<pin name="1" x="7.62" y="2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="2" x="7.62" y="0" visible="pad" length="short" direction="pas" rot="R180"/>
<pin name="3" x="7.62" y="-2.54" visible="pad" length="short" direction="pas" rot="R180"/>
<polygon width="0.1524" layer="94">
<vertex x="2.54" y="-2.54"/>
<vertex x="2.032" y="-1.27"/>
<vertex x="3.048" y="-1.27"/>
</polygon>
</symbol>
</symbols>
<devicesets>
<deviceset name="DCJ0303" prefix="J" uservalue="yes">
<description>&lt;b&gt;DC POWER JACK&lt;/b&gt;&lt;p&gt;
Source: DCJ0303.pdf</description>
<gates>
<gate name="G$1" symbol="DC-JACK-SWITCH" x="0" y="0"/>
</gates>
<devices>
<device name="" package="DCJ0303">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
</connects>
<technologies>
<technology name="">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="unknown" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="LED1" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED2" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED3" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED4" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED5" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED6" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED7" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED8" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED9" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED10" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED11" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED12" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED13" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED14" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="LED15" library="led" deviceset="MULTILED_LRTB_G6TG" device=""/>
<part name="J2" library="con-jack" deviceset="DCJ0303" device=""/>
<part name="J1" library="con-jack" deviceset="DCJ0303" device="">
<attribute name="MF" value=""/>
<attribute name="MPN" value=""/>
<attribute name="OC_FARNELL" value="unknown"/>
<attribute name="OC_NEWARK" value="unknown"/>
</part>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="LED1" gate="R" x="25.4" y="86.36"/>
<instance part="LED1" gate="G" x="35.56" y="86.36"/>
<instance part="LED1" gate="B" x="45.72" y="86.36"/>
<instance part="LED2" gate="R" x="25.4" y="78.74"/>
<instance part="LED2" gate="G" x="35.56" y="78.74"/>
<instance part="LED2" gate="B" x="45.72" y="78.74"/>
<instance part="LED3" gate="R" x="25.4" y="71.12"/>
<instance part="LED3" gate="G" x="35.56" y="71.12"/>
<instance part="LED3" gate="B" x="45.72" y="71.12"/>
<instance part="LED4" gate="R" x="66.04" y="86.36"/>
<instance part="LED4" gate="G" x="76.2" y="86.36"/>
<instance part="LED4" gate="B" x="86.36" y="86.36"/>
<instance part="LED5" gate="R" x="66.04" y="78.74"/>
<instance part="LED5" gate="G" x="76.2" y="78.74"/>
<instance part="LED5" gate="B" x="86.36" y="78.74"/>
<instance part="LED6" gate="R" x="66.04" y="71.12"/>
<instance part="LED6" gate="G" x="76.2" y="71.12"/>
<instance part="LED6" gate="B" x="86.36" y="71.12"/>
<instance part="LED7" gate="R" x="109.22" y="86.36"/>
<instance part="LED7" gate="G" x="119.38" y="86.36"/>
<instance part="LED7" gate="B" x="129.54" y="86.36"/>
<instance part="LED8" gate="R" x="109.22" y="78.74"/>
<instance part="LED8" gate="G" x="119.38" y="78.74"/>
<instance part="LED8" gate="B" x="129.54" y="78.74"/>
<instance part="LED9" gate="R" x="109.22" y="71.12"/>
<instance part="LED9" gate="G" x="119.38" y="71.12"/>
<instance part="LED9" gate="B" x="129.54" y="71.12"/>
<instance part="LED10" gate="R" x="25.4" y="33.02"/>
<instance part="LED10" gate="G" x="35.56" y="33.02"/>
<instance part="LED10" gate="B" x="45.72" y="33.02"/>
<instance part="LED11" gate="R" x="25.4" y="25.4"/>
<instance part="LED11" gate="G" x="35.56" y="25.4"/>
<instance part="LED11" gate="B" x="45.72" y="25.4"/>
<instance part="LED12" gate="R" x="25.4" y="17.78"/>
<instance part="LED12" gate="G" x="35.56" y="17.78"/>
<instance part="LED12" gate="B" x="45.72" y="17.78"/>
<instance part="LED13" gate="R" x="68.58" y="38.1"/>
<instance part="LED13" gate="G" x="78.74" y="38.1"/>
<instance part="LED13" gate="B" x="88.9" y="38.1"/>
<instance part="LED14" gate="R" x="68.58" y="30.48"/>
<instance part="LED14" gate="G" x="78.74" y="30.48"/>
<instance part="LED14" gate="B" x="88.9" y="30.48"/>
<instance part="LED15" gate="R" x="68.58" y="22.86"/>
<instance part="LED15" gate="G" x="78.74" y="22.86"/>
<instance part="LED15" gate="B" x="88.9" y="22.86"/>
<instance part="J2" gate="G$1" x="-2.54" y="73.66"/>
<instance part="J1" gate="G$1" x="-5.08" y="63.5"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="LED2" gate="R" pin="A"/>
<pinref part="LED1" gate="R" pin="C"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="LED2" gate="G" pin="A"/>
<pinref part="LED1" gate="G" pin="C"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="LED2" gate="B" pin="A"/>
<pinref part="LED1" gate="B" pin="C"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="LED2" gate="B" pin="C"/>
<pinref part="LED3" gate="B" pin="A"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="LED2" gate="G" pin="C"/>
<pinref part="LED3" gate="G" pin="A"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="LED2" gate="R" pin="C"/>
<pinref part="LED3" gate="R" pin="A"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="LED5" gate="R" pin="A"/>
<pinref part="LED4" gate="R" pin="C"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="LED5" gate="G" pin="A"/>
<pinref part="LED4" gate="G" pin="C"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="LED5" gate="B" pin="A"/>
<pinref part="LED4" gate="B" pin="C"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="LED5" gate="B" pin="C"/>
<pinref part="LED6" gate="B" pin="A"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="LED5" gate="G" pin="C"/>
<pinref part="LED6" gate="G" pin="A"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="LED5" gate="R" pin="C"/>
<pinref part="LED6" gate="R" pin="A"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="LED8" gate="R" pin="A"/>
<pinref part="LED7" gate="R" pin="C"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="LED8" gate="G" pin="A"/>
<pinref part="LED7" gate="G" pin="C"/>
</segment>
</net>
<net name="N$15" class="0">
<segment>
<pinref part="LED8" gate="B" pin="A"/>
<pinref part="LED7" gate="B" pin="C"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<pinref part="LED8" gate="R" pin="C"/>
<pinref part="LED9" gate="R" pin="A"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="LED8" gate="G" pin="C"/>
<pinref part="LED9" gate="G" pin="A"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<pinref part="LED8" gate="B" pin="C"/>
<pinref part="LED9" gate="B" pin="A"/>
</segment>
</net>
<net name="N$19" class="0">
<segment>
<pinref part="LED11" gate="R" pin="C"/>
<pinref part="LED12" gate="R" pin="A"/>
</segment>
</net>
<net name="N$20" class="0">
<segment>
<pinref part="LED11" gate="R" pin="A"/>
<pinref part="LED10" gate="R" pin="C"/>
</segment>
</net>
<net name="N$21" class="0">
<segment>
<pinref part="LED11" gate="G" pin="A"/>
<pinref part="LED10" gate="G" pin="C"/>
</segment>
</net>
<net name="N$22" class="0">
<segment>
<pinref part="LED11" gate="G" pin="C"/>
<pinref part="LED12" gate="G" pin="A"/>
</segment>
</net>
<net name="N$23" class="0">
<segment>
<pinref part="LED11" gate="B" pin="A"/>
<pinref part="LED10" gate="B" pin="C"/>
</segment>
</net>
<net name="N$24" class="0">
<segment>
<pinref part="LED11" gate="B" pin="C"/>
<pinref part="LED12" gate="B" pin="A"/>
</segment>
</net>
<net name="N$25" class="0">
<segment>
<pinref part="LED13" gate="R" pin="C"/>
<pinref part="LED14" gate="R" pin="A"/>
</segment>
</net>
<net name="N$26" class="0">
<segment>
<pinref part="LED13" gate="G" pin="C"/>
<pinref part="LED14" gate="G" pin="A"/>
</segment>
</net>
<net name="N$27" class="0">
<segment>
<pinref part="LED13" gate="B" pin="C"/>
<pinref part="LED14" gate="B" pin="A"/>
</segment>
</net>
<net name="N$28" class="0">
<segment>
<pinref part="LED14" gate="R" pin="C"/>
<pinref part="LED15" gate="R" pin="A"/>
</segment>
</net>
<net name="N$29" class="0">
<segment>
<pinref part="LED14" gate="G" pin="C"/>
<pinref part="LED15" gate="G" pin="A"/>
</segment>
</net>
<net name="N$30" class="0">
<segment>
<pinref part="LED14" gate="B" pin="C"/>
<pinref part="LED15" gate="B" pin="A"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
