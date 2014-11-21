# <img src="https://s3-eu-west-1.amazonaws.com/biipublicimage/lasote/guybrush_point.gif" width="50"> lasote/arduino_http

### What is it?
This is a simple but powerfull Arduino Client library. It works with low memory requirements using processors based on streaming text recognition. Only stores the very necessary data for work. Supports GET and POST requests.

### Tested on

Its tested using a <a href="http://www.ebay.com/sch/i.html?_odkw=enc28j60+ethernet+lan+network+module&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xenc28j60+ethernet+lan+network+module+arduino&_nkw=enc28j60+ethernet+lan+network+module+arduino&_sacat=0">cheap Ethernet Enc28j60 module</a>.

HttpClient can work with other adapters that implements Client interface.

<table border="1" cellpadding="10">
    <tr>
        <td width="40%">**Board**</td>
        <td width="60%">**Example**</td>
    </tr>
    <tr>
        <td> <a href="http://www.ebay.com/sch/i.html?_odkw=enc28j60+ethernet+lan+network+module&_osacat=0&_from=R40&_trksid=p2045573.m570.l1313.TR0.TRC0.H0.Xenc28j60+ethernet+lan+network+module+arduino&_nkw=enc28j60+ethernet+lan+network+module+arduino&_sacat=0">Enc28j60 no-shield</a> </td>
        <td><a href="https://www.biicode.com/lasote/blocks/lasote/arduino_http/branches/master/versions/13/cells/example/menu_detector.cp</<>p"> here</a></td>
    </tr>
    <tr>
        <td> <a href="https://www.adafruit.com/products/1469">Adafruit Wifi cc3000</a></td>
	<td> How to instance a client <a href="https://www.biicode.com/diego/blocks/diego/ardunet/branches/master/versions/1/cells/readme.md">here</a>.<br> Complete example coming soon...</td>
    </tr>
    <tr>
        <td><a href="http://arduino.cc/es/Main/ArduinoEthernetShield#.U0VR1Be9g8Q">Arduino Ethernet Shield</a></td>
        <td>Coming soon...</td>
    </tr>
    <tr>
        <td><a href=" http://arduino.cc/en/Main/ArduinoWiFiShield">Arduino Wifi Shield</a></td>
        <td>Coming soon...</td>
    </tr>
    
   
</table>

### How can i use it?

Take a look to the <a href="https://www.biicode.com/lasote/blocks/lasote/arduino_http/branches/maste">*example/post.cpp* and *example/menu_detector*</a> for full examples.  
Common steps are:
* Instance the _Client object_ for your hardware adapter
* Instance an HttpClient passing on the _Client object_
* Write init code of your _Client object_ in Setup method
* Just call *http_client.post* or *http_client.get*
    

        EthernetClient client;
        HttpClient http_client(client, Serial);
        //Create the processors you want in order to parse the response
        StreamRecorderProcessor response_recorder(response_buffer, 100); //Capture response in buffer
        ...
        //SETUP: Add the processors to the http_client
        http_client.processors.add_item(&response_recorder);
        
        //LOOP:
        http_client.reset(); //Reset previous processors status and custom headers etc.
        http_client.get("www.google.com", "/webhp#q=biicode");
        ...
        http_client.receive()
        ...
        //Now check the processor for the result!
        
        

### Advanced uses

#### Using processors

As I mentioned previously, this http client is based on stream processors for response parse in order to keep as low as possible memory requirements. This is a list of util processors.
<table border="1" cellpadding="10">
    <tr>
        <td width="20%">**Name**</td>
        <td width="40%">**Description**</td>
        <td width="40%">**Include**</td>        
    </tr>
    <tr>
        <td><a href="https://www.biicode.com/lasote/blocks/lasote/stream_json_reader/branches/master">StreamJsonProcessor</a></td>
        <td>Parse json from http response. You have an example in <a href="https://www.biicode.com/lasote/blocks/lasote/weather/branches/master">arduino_weather project</a>. <br>Based on <a href="https://www.biicode.com/lasote/blocks/lasote/stream_json_reader/branches/master">StreamJsonReader</a></td>
        <td>#include "lasote/stream_processor/stream_json_processor.h"</td>        
    </tr>
    <tr>
        <td>StreamFinderProcessor</td>
        <td>Checks if a string in exists in a response. Based on a <a href="https://www.biicode.com/lasote/blocks/lasote/stream_finder/branches/master"> StreamFinder</a>. Check the menu_detector example in <a href="https://www.biicode.com/lasote/blocks/lasote/arduino_http/branches/master">block home page</a></td>
        <td>#include "lasote/stream_finder_processor/stream_finder_processor.h"</td>                
    </tr>
    <tr>
        <td>StreamRecorderProcessor</td>
        <td>Just record the whole response in a buffer. Check the post example</td>
        <td>#include "lasote/stream_recorder/stream_recorder.h"</td>        
    </tr>      
</table>

#### Using POST request

Look the post.cpp example for POST request usage.


