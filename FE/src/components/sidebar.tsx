import * as React from "react";
import {
  Drawer,
  List,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Toolbar,
  Divider,
  Box,
} from "@mui/material";
import HomeIcon from "@mui/icons-material/Home";
import PeopleIcon from "@mui/icons-material/People";
import SettingsIcon from "@mui/icons-material/Settings";
import LogoutIcon from "@mui/icons-material/Logout";
import { Link, useNavigate } from "react-router-dom";

const drawerWidth = 240;
const collapsedWidth = 64;

export default function Sidebar({ open }) {
  const navigate = useNavigate();

  const menuItems = [
    { text: "Home", icon: <HomeIcon />, path: "/dashboard/home" },
    { text: "Users", icon: <PeopleIcon />, path: "/dashboard/users" },
    { text: "Settings", icon: <SettingsIcon />, path: "/dashboard/settings" },
  ];

  const handleLogout = () => {
    localStorage.removeItem('accessToken');
    localStorage.removeItem('user');

    sessionStorage.removeItem('accessToken');
    sessionStorage.removeItem('user');
    navigate("/login");
  };

  return (
    <Drawer
      variant="permanent"
      sx={{
        width: open ? drawerWidth : collapsedWidth,
        flexShrink: 0,
        "& .MuiDrawer-paper": {
          width: open ? drawerWidth : collapsedWidth,
          boxSizing: "border-box",
          backgroundColor: "#1e1e2f",
          color: "#fff",
          transition: "width 0.3s ease",
          overflowX: "hidden",
        },
      }}
    >
      <Toolbar sx={{ justifyContent: "center" }}>
        <h3>{open ? "Iot Dashboard" : "IOT"}</h3>
      </Toolbar>
      <Divider />

      <List>
        {menuItems.map((item) => (
          <ListItemButton
            key={item.text}
            component={Link}
            to={item.path}
            sx={{
              "&:hover": { backgroundColor: "#2c2c45" },
              color: "#fff",
            }}
          >
            <ListItemIcon sx={{ color: "#61dafb" }}>{item.icon}</ListItemIcon>
            <ListItemText
              primary={item.text}
              sx={{ display: open ? "block" : "none" }}
            />
          </ListItemButton>
        ))}
      </List>

      <Divider />

      <Box sx={{ flexGrow: 1 }} />

      <List sx={{ mt: "auto" }}>
        <ListItemButton
          onClick={handleLogout}
          sx={{
            "&:hover": { backgroundColor: "#2c2c45" },
            color: "#fff",
          }}
        >
          <ListItemIcon sx={{ color: "#ff5555" }}>
            <LogoutIcon />
          </ListItemIcon>
          <ListItemText
            primary="Logout"
            sx={{ display: open ? "block" : "none" }}
          />
        </ListItemButton>
      </List>
    </Drawer>
  );
}
