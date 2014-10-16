<%-- 
    Document   : service
    Created on : Jul 12, 2014, 1:07:38 PM
    Author     : savelazq
--%>

<%@page import="java.util.Enumeration"%>
<%@page import="org.json.simple.JSONValue"%>
<%@page contentType="text/html; charset=UTF-8"%>
<%!
    String eventtype = "SMOKE";
    String helmetName = "Helmet 10";
%>


<script src="../public/js/jquery-1.9.1.min.js"></script>
<script src="../public/js/quickblox.js"></script>
<script type="text/javascript">
  
    //var QB = require('quickblox');
    if("<%=eventtype%>" === "SMOKE") {
        sendMessage("Smoke detected on <%=helmetName%>");
    } else if("<%=eventtype%>" === "STRIKE") {
        sendMessage("Strike detected on <%=helmetName%>");
    } else if("<%=eventtype%>" === "FALL") {
        sendMessage("Fall detected on <%=helmetName%>");
    }  
    
    function sendMessage(message) {
        appId = 13839;
        authKey = "LJLQk5GFvwPr-uJ";
        secret = "p6-ntPbsp5W5n24";
        user = "edison_helmet";
        password = "edisonhelmet";
        QB.init(appId,authKey,secret, true);
        QB.createSession({login: user, password: password}, function(err, result){
            if (err) {
                console.log("session error");
            } else {
                console.log("session created");
                sendEvent(message);
            }
        });
    }
    
    function sendEvent(message) {
        var event = {
            notification_type: "push",
            push_type: "gcm",
            environment: "development",
            message: window.btoa(unescape(encodeURIComponent(message))),
            event_type: "one_shot"
        };
        QB.messages.events.create(event, function (err, result){
            if (err) {
                console.log("event error");
            } else {
                console.log("event created");
            }
        });
    }
</script>
<script src="../public/js/jquery-1.9.1.min.js"></script>
<script src="../public/js/quickblox.js"></script>
<script type="text/javascript">
  
    //var QB = require('quickblox');
    if("<%=eventtype%>" === "SMOKE") {
        sendMessage("Smoke detected on <%=helmetName%>");
    } else if("<%=eventtype%>" === "STRIKE") {
        sendMessage("Strike detected on <%=helmetName%>");
    } else if("<%=eventtype%>" === "FALL") {
        sendMessage("Fall detected on <%=helmetName%>");
    }  
    
    function sendMessage(message) {
        appId = 13839;
        authKey = "LJLQk5GFvwPr-uJ";
        secret = "p6-ntPbsp5W5n24";
        user = "edison_helmet";
        password = "edisonhelmet";
        QB.init(appId,authKey,secret, true);
        QB.createSession({login: user, password: password}, function(err, result){
            if (err) {
                console.log("session error");
            } else {
                console.log("session created");
                sendEvent(message);
            }
        });
    }
    
    function sendEvent(message) {
        var event = {
            notification_type: "push",
            push_type: "gcm",
            environment: "development",
            message: window.btoa(unescape(encodeURIComponent(message))),
            event_type: "one_shot"
        };
        QB.messages.events.create(event, function (err, result){
            if (err) {
                console.log("event error");
            } else {
                console.log("event created");
            }
        });
    }
</script>