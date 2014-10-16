<%-- 
    Document   : service
    Created on : Jul 12, 2014, 1:07:38 PM
    Author     : savelazq
--%>

<%@page contentType="text/html; charset=UTF-8"%>
<%@ page import="java.sql.*" %>
<%@page import="org.json.simple.JSONObject"%>
<%@page import="org.json.simple.JSONArray"%>
<%!
            Connection conConexion;
            Statement scSQL;
            ResultSet rsListaRegistros;
            ResultSetMetaData lsDatos;
        %>
<%
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            conConexion = DriverManager.getConnection("jdbc:mysql://localhost/edison_helmet",
                    "helmet", "helmet");
            scSQL = conConexion.createStatement();
            rsListaRegistros = scSQL.executeQuery("SELECT mac_address,name,event_type,value,gps_status,gps_latitude,gps_longitude,gps_altitude,event_date  FROM events, helmets where events.helmet_id=helmets.id ORDER BY events.id desc LIMIT 20");
            lsDatos = rsListaRegistros.getMetaData();
            JSONArray jsona= new JSONArray();           
            while(rsListaRegistros.next())
            {
              JSONObject obj = new JSONObject();
              int nCols = lsDatos.getColumnCount();
              for ( int i=1; i<nCols+1; i++){
                  String column_name= lsDatos.getColumnName(i);
                  obj.put(column_name,rsListaRegistros.getString(i));
              }
              jsona.add(obj);
            }
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            response.getWriter().print(jsona);
        %>