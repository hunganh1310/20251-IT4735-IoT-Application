import { Outlet } from "react-router-dom";
import { Box, IconButton, Toolbar, AppBar, Typography, Badge, Avatar } from "@mui/material";
import MenuIcon from "@mui/icons-material/Menu";
import NotificationsIcon from "@mui/icons-material/Notifications";
import Sidebar from "../components/sidebar";
import ProfileCard from "../components/profile-card";
import { useState } from "react";

const drawerWidth = 240;

export default function Dashboard() {
  const [open, setOpen] = useState(true);
  
  const [anchorEl, setAnchorEl] = useState(null);
  const openMenu = Boolean(anchorEl);

  const [user, setUser] = useState(() => {
      let savedUser = localStorage.getItem("user");
      if (!savedUser) {
        savedUser = sessionStorage.getItem("user");
      }
      return savedUser ? JSON.parse(savedUser) : {};
  });

  const handleToggleSidebar = () => setOpen(!open);

  console.log("Dữ liệu user trong React:", user);

  const handleUserUpdated = (newUser) => {
      setUser(newUser);
  };

  const handleAvatarClick = (event) => {
    setAnchorEl(event.currentTarget);
  };

  const handleMenuClose = () => {
    setAnchorEl(null);
  };

  return (
    <Box sx={{ display: "flex" }}>
      <Sidebar open={open} />

      <AppBar
        position="fixed"
        sx={{
          zIndex: (theme) => theme.zIndex.drawer + 1,
          bgcolor: "#fff",
          color: "#333",
          boxShadow: "none",
          borderBottom: "1px solid #ddd",
          transition: "margin 0.3s ease, width 0.3s ease",
          ...(open && {
            marginLeft: `${drawerWidth}px`,
            width: `calc(100% - ${drawerWidth}px)`,
          }),
        }}
      >
        <Toolbar>
          <IconButton color="inherit" edge="start" onClick={handleToggleSidebar} sx={{ mr: 2 }}>
            <MenuIcon />
          </IconButton>
          <Typography variant="h6" noWrap>Dashboard</Typography>

          <Box sx={{ flexGrow: 1 }} />

          <IconButton color="inherit" sx={{ mr: 2 }}>
            <Badge badgeContent={4} color="error">
              <NotificationsIcon />
            </Badge>
          </IconButton>
          
          <IconButton sx={{ p: 0 }} onClick={handleAvatarClick}>
            <Avatar alt={user?.name} src={user?.avatarUrl} sx={{ bgcolor: '#1976d2' }}>
              {user?.name?.charAt(0).toUpperCase()}
            </Avatar>
            <Typography variant="body1" sx={{ ml: 1, color: "#333" }}>
              {user?.name || "Guest"}
            </Typography>
          </IconButton>

          <ProfileCard 
            anchorEl={anchorEl} 
            open={openMenu} 
            onClose={handleMenuClose} 
            user={user}
            onUserUpdated={handleUserUpdated}
          />

        </Toolbar>
      </AppBar>

      <Box component="main" sx={{ flexGrow: 1, bgcolor: "#f5f5f5", minHeight: "100vh", transition: "margin 0.3s ease" }}>
        <Toolbar />
        <Outlet context={handleUserUpdated} />
      </Box>
    </Box>
  );
}