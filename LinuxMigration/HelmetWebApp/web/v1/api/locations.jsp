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
            ResultSet rshelmet;
            ResultSetMetaData lsDatos,helmets;
            String helmet_id;
        %>
<%
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            conConexion = DriverManager.getConnection("jdbc:mysql://localhost/edison_helmet",
                    "helmet", "helmet");
            scSQL = conConexion.createStatement();
            //rsListaRegistros = scSQL.executeQuery("SELECT name,event_type,value,gps_status,gps_latitude,gps_longitude,gps_altitude,event_date  FROM events, helmets where events.helmet_id=helmets.id and is_active=1 order by event_date top 1 ");
            //rsListaRegistros = scSQL.executeQuery(" SELECT name,gps_latitude,gps_longitude,gps_altitude from event, helmets where events.helmet_id= ? order by event_date LIMIT 1");
            rshelmet = scSQL.executeQuery("SELECT id FROM helmets WHERE is_active=1 and id = 9");
            JSONArray jsona= new JSONArray(); 
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            while(rshelmet.next())
            {
              ResultSet rsListaRegistros;
              scSQL = conConexion.createStatement();
              helmet_id = rshelmet.getString(1);
              String query = "SELECT helmets.id, name,gps_latitude,gps_longitude from events, helmets where events.helmet_id=" +helmet_id +" and events.helmet_id = helmets.id order by event_date DESC LIMIT 1";
              rsListaRegistros = scSQL.executeQuery(query);
              //response.getWriter().println(query);
              lsDatos = rsListaRegistros.getMetaData();
              while(rsListaRegistros.next())
              {
                JSONObject obj = new JSONObject();
                int nCols = lsDatos.getColumnCount();
                for ( int i=1; i<nCols+1; i++){
                  String column_name= lsDatos.getColumnName(i);
                  obj.put(column_name,rsListaRegistros.getString(i));                                 }
                jsona.add(obj);
              }
            
            }
            response.getWriter().println(jsona);
        %>