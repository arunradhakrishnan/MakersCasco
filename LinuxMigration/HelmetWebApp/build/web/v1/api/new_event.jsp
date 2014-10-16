<%-- 
    Document   : service
    Created on : Jul 12, 2014, 1:07:38 PM
    Author     : savelazq
--%>

<%@page import="java.util.Enumeration"%>
<%@page import="org.json.simple.JSONValue"%>
<%@page contentType="text/html; charset=UTF-8"%>
<%@ page import="java.sql.*" %>
<%@page import="org.json.simple.JSONObject"%>
<%@page import="org.json.simple.JSONArray"%>
<%!
            Connection conConexion;
            Statement scSQL;
            ResultSet rsListaRegistros;
            ResultSetMetaData lsDatos;
            String macaddress;
            String eventtype;
            String value;
            String gpsstatus;
            String gpslatitude;
            String gpslongitude;
            String gpsalitude;
            String helmetId;
            String helmetName;
            int updateQuery;
%>
<%
            String json=request.getParameterNames().nextElement();
            System.out.println(json);
            JSONObject jsonData = (JSONObject) JSONValue.parse(json);
            macaddress = (String)jsonData.get("mac_address");
            eventtype = (String) jsonData.get("event_type");
            value = (String) jsonData.get("value");
            gpsstatus = (String) jsonData.get("gps_status");
            gpslatitude = (String) jsonData.get("gps_latitude"); 
            if(gpslatitude.isEmpty()) gpslatitude = "0";
            gpslongitude = (String) jsonData.get("gps_longitude"); 
            if(gpslongitude.isEmpty()) gpslongitude = "0";
            gpsalitude = (String) jsonData.get("gps_altitude");
            if(gpsalitude.isEmpty()) gpsalitude = "0";
            
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            response.setStatus(200);
            //response.getWriter().println(macaddress);
            
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            conConexion = DriverManager.getConnection("jdbc:mysql://localhost/edison_helmet",
                    "helmet", "helmet");
            
            String query = "SELECT id, name FROM helmets where mac_address = '" + macaddress + "'";
            System.out.println(query);
            scSQL = conConexion.createStatement();
            ResultSet helmet = scSQL.executeQuery(query);
            helmet.next();
            helmetId = helmet.getString("id");
            helmetName = helmet.getString("name");
            
            // harcoded
            
            //gpslatitude = "37.783217";
            //gpslongitude = "-122.404204";
            //gpsalitude = "1736.70";
            
            //gpslatitude = "40.7452331";
            //gpslongitude = "-74.00703";
            //gpsalitude = "50.70";
            
            
            PreparedStatement pstatement=null;
            scSQL = conConexion.createStatement();
            String queryString="INSERT INTO events (id,helmet_id,event_type,value,gps_status,gps_latitude,gps_longitude, gps_altitude, event_date) VALUES(DEFAULT,?,?,?,?,?,?,?,NOW())";
            pstatement = conConexion.prepareStatement(queryString);
            pstatement.setString(1, helmetId);
            pstatement.setString(2, eventtype);
            pstatement.setString(3, value);
            pstatement.setString(4, gpsstatus);
            pstatement.setString(5, gpslatitude);
            pstatement.setString(6, gpslongitude);
            pstatement.setString(7, gpsalitude);
            System.out.println(pstatement.toString());
            updateQuery= pstatement.executeUpdate();
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            response.setStatus(200);
            response.getWriter().println(json);
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