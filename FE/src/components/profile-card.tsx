import { Box, Button, TextField, IconButton, Avatar, Menu, Typography } from "@mui/material";
import EditIcon from "@mui/icons-material/Edit";
import { useState, useEffect } from "react";

interface ProfileCardProps {
  anchorEl: null | HTMLElement;
  open: boolean;
  onClose: () => void;
  user: any;
}

const ProfileCard = ({ anchorEl, open, onClose, user, onUserUpdated }) => {
  const [isEditing, setIsEditing] = useState(false);
  const [name, setName] = useState("");
  const [email, setEmail] = useState("");

  useEffect(() => {
    if (open) {
      setName(user?.name || "");
      setEmail(user?.email || "");
      setIsEditing(false);
    }
  }, [user, open]);

  const handleEditClick = () => {
    setIsEditing(true);
  };

  const handleCancel = () => {
    if (isEditing) {
      setIsEditing(false);
      setName(user?.name || "");
      setEmail(user?.email || "");
    } else {
      onClose();
    }
  };

  const handleSave = () => {
    console.log("Saving...", name, email);
    
    const updatedUser = { ...user, name, email };
    localStorage.setItem("user", JSON.stringify(updatedUser));

    if (onUserUpdated) {
        onUserUpdated(updatedUser);
    }
    setIsEditing(false);
    onClose();
  };

  return (
    <Menu
      anchorEl={anchorEl}
      open={open}
      onClose={onClose}
      PaperProps={{
        elevation: 3,
        sx: {
          mt: 1.5,
          width: 320,
          overflow: "visible",
          "&:before": {
            content: '""',
            display: "block",
            position: "absolute",
            top: 0,
            right: 14,
            width: 10,
            height: 10,
            bgcolor: "background.paper",
            transform: "translateY(-50%) rotate(45deg)",
            zIndex: 0,
          },
        },
      }}
      transformOrigin={{ horizontal: "right", vertical: "top" }}
      anchorOrigin={{ horizontal: "right", vertical: "bottom" }}
    >
      <Box sx={{ p: 3, display: "flex", flexDirection: "column", alignItems: "center", position: "relative" }}>
        
        <IconButton
          size="small"
          onClick={isEditing ? handleCancel : handleEditClick}
          sx={{
            position: "absolute",
            top: 0,
            right: 0,
            color: isEditing ? "primary.main" : "action.active",
          }}
          title={isEditing ? "Hủy chỉnh sửa" : "Chỉnh sửa thông tin"}
        >
          <EditIcon />
        </IconButton>

        {/* Avatar */}
        <Avatar
          alt={user?.name}
          src={user?.avatarUrl}
          sx={{ width: 80, height: 80, bgcolor: "#1976d2", mb:0, fontSize: "2rem" }}
        >
          {user?.name?.charAt(0).toUpperCase()}
        </Avatar>

        <Box sx={{ textAlign: 'center', mb: 2 }}>
            <Typography variant="h6" fontWeight="bold">
                {user?.name || "Guest"}
            </Typography>
        </Box>

        {/* Form nhập liệu */}
        <Box sx={{ width: "100%", display: "flex", flexDirection: "column", gap: 2 }}>
          <TextField
            label="Tên hiển thị"
            fullWidth
            size="small"
            value={name}
            onChange={(e) => setName(e.target.value)}
            disabled={!isEditing}
          />
          <TextField
            label="Email"
            fullWidth
            size="small"
            value={email}
            onChange={(e) => setEmail(e.target.value)}
            disabled={!isEditing}
          />
        </Box>

        {/* Nút hành động */}
        <Box sx={{ display: "flex", gap: 1, mt: 3, width: "100%", justifyContent: "flex-end" }}>
          <Button variant="outlined" size="small" onClick={handleCancel}>
            Cancel
          </Button>

          <Button
            variant="contained"
            size="small"
            onClick={handleSave}
            disabled={!isEditing}
          >
            Save
          </Button>
        </Box>
      </Box>
    </Menu>
  );
};

export default ProfileCard;