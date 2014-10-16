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
            String id;
            int updateQuery;
        %>
<%
            String id=request.getParameter("id");
            //JSONObject jsonData = (JSONObject) JSONValue.parse(json);
            //id = (String)jsonData.get("id");
            
            
            Class.forName("com.mysql.jdbc.Driver").newInstance();
            conConexion = DriverManager.getConnection("jdbc:mysql://localhost/edison_helmet",
                    "helmet", "helmet");
            PreparedStatement pstatement=null;
            scSQL = conConexion.createStatement();
            //String queryString="DELETE FROM helmets WHERE id="+id;
            String queryString="UPDATE helmets SET is_active=0 WHERE id=?";
            pstatement = conConexion.prepareStatement(queryString);
            pstatement.setString(1, id);
            updateQuery= pstatement.executeUpdate(); 
            response.setCharacterEncoding("UTF-8");
            response.setContentType("application/json");
            response.setStatus(200);
            response.getWriter().println("{\"status\":\"sucess\"}");
        %>
