import { Box, IconButton, Avatar, Menu, Typography } from "@mui/material";
import EditIcon from "@mui/icons-material/Edit";
import { useNavigate } from "react-router-dom";

interface ProfileCardProps {
  anchorEl: null | HTMLElement;
  open: boolean;
  onClose: () => void;
  user: any;
}

const ProfileCard = ({ anchorEl, open, onClose, user }: ProfileCardProps) => {
  const navigate  = useNavigate();

  const handleEditClick = () => {
    onClose();
    navigate("/dashboard/settings/");
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
          onClick={handleEditClick}
          sx={{
            position: "absolute",
            top: 0,
            right: 0,
            color: "action.active",
          }}
          title="Chỉnh sửa thông tin"
        >
          <EditIcon />
        </IconButton>

        {/* Avatar */}
        <Avatar
          alt={user?.name}
          src={user?.avatarUrl}
          sx={{ width: 80, height: 80, bgcolor: "#1976d2", fontSize: "2rem" }}
        >
          {user?.name?.charAt(0).toUpperCase()}
        </Avatar>

        <Box sx={{ textAlign: 'center', mb: 2 }}>
            <Typography variant="h6" fontWeight="bold">
                {user?.name || "Guest"}
            </Typography>
            <Typography variant="body2" color="text.secondary">
                {user?.email || "No email provided"}
            </Typography>
        </Box>
      </Box>
    </Menu>
  );
};

export default ProfileCard;