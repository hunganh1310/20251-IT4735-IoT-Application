import { Outlet } from "react-router-dom";
import { Box, IconButton, Toolbar, AppBar, Typography } from "@mui/material";
import MenuIcon from "@mui/icons-material/Menu";
import Sidebar from "../components/sidebar";
import { useState } from "react";

const drawerWidth = 240; // chiều rộng sidebar của bạn

export default function Dashboard() {
  const [open, setOpen] = useState(true);

  const handleToggleSidebar = () => {
    setOpen(!open);
  };

  return (
    <Box sx={{ display: "flex" }}>
      {/* Sidebar */}
      <Sidebar open={open} />

      {/* AppBar */}
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
          <IconButton
            color="inherit"
            edge="start"
            onClick={handleToggleSidebar}
            sx={{ mr: 2 }}
          >
            <MenuIcon />
          </IconButton>
          <Typography variant="h6" noWrap>
            Dashboard
          </Typography>
        </Toolbar>
      </AppBar>

      {/* Main content */}
      <Box
        component="main"
        sx={{
          flexGrow: 1,
          bgcolor: "#f5f5f5",
          minHeight: "100vh",
          transition: "margin 0.3s ease",
        }}
      >
        <Toolbar /> {/* để tránh bị AppBar đè lên nội dung */}
        <Outlet />
      </Box>
    </Box>
  );
}
